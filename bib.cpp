#include "lib.h"
#include <QRegularExpression>
#include <functional>

namespace bib
{

void Bib::fromFile(const QString & filePath)
{
	QFile inFile(filePath);
	inFile.open(QIODevice::ReadOnly);
	QString contents = inFile.readAll();
	inFile.close();

	this->fromContents(contents);
}

void Bib::fromContents(const QString & bibContents)
{
	/// set style
	QString sta = bibContents.mid(1, 20);
	if(sta.startsWith(article))
	{
		this->format = Style::article;
		this->style = bib::articleStyle;
	}
	else if(sta.startsWith(book))
	{
		this->format = Style::book;
		this->style = bib::bookStyle;
	}
	else if(sta.startsWith(conference))
	{
		this->format = Style::conference;
		this->style = bib::confStyle;
	}
	else if(sta.startsWith(inproceedings))
	{
		this->format = Style::proceedings;
		this->style = bib::procStyle;
	}
	else if(sta.startsWith(chapter))
	{
		this->format = Style::chapter;
		this->style = bib::chapterStyle;
	}
	else
	{
		this->format = Style::unknown;
		this->style = bib::unknownStyle;
	}

	for(const auto & acr : bib::styleAcronyms)
	{
		QRegularExpression toFind{"(?<=" + acr.second + R"(\=\{)(.*)(?=\}))"};
		auto mat = toFind.match(bibContents);
		if(mat.hasMatch())
		{
			this->dt[acr.second] = mat.captured();
		}
	}

	/// chop last dot in title
	if(dt[title].endsWith('.')) { dt[title].chop(1); }

	this->authors = bib::authorsFromData(this->dt[author]);
}

Bib::Bib(const QString & bibContents)
{
	fromContents(bibContents);
}


std::vector<std::vector<QString>> authorsFromData(const QString & authors)
{
	QStringList lst = authors.split(" and ", QString::SkipEmptyParts);
	std::vector<std::vector<QString>> names{}; /// [authNum][last/first/second(s)]

	for(const QString & in : lst)
	{
		QString first{};
		QString last{};
		std::vector<QString> second{};

		if(in.contains(',')) /// Last, First Second - usual format
		{
			last = in.left(in.indexOf(','));		/// set last (family) name
			QString in2 = in.mid(in.indexOf(','));	/// cut last name

			auto ll = in2.split(QRegExp(R"([, ])"), QString::SkipEmptyParts); /// only spaces

			/// sometimes first and second names are concatenated by first letters
			/// sometimes they are more than 2
			/// what with cyrillic???

			if(ll.size() == 1 && ll[0].toUpper() == ll[0]) /// all letters are upper
			{
				first = ll[0][0];				/// first name is the first letter
				QString tmp = ll[0].mid(1);		/// second names
				for(const auto & in : tmp)
				{
					second.push_back(in);
				}
			}
			else /// there are divided sfirst and second names
			{
				first = ll[0];
				for(int i = 1; i < ll.size(); ++i)
				{
					second.push_back(ll[i]);
				}
			}
		}
		else /// First Second(s) Last - unusual case
		{
			auto l = in.split(' ', QString::SkipEmptyParts);
			last = l.last();
			first = l.first();
			for(int i = 1; i < l.size() - 1; ++i)
			{
				second.push_back(l[i]);
			}
		}

		std::vector<QString> tmp{};
		tmp.push_back(last);
		tmp.push_back(first);
		for(const QString & in : second)
		{
			tmp.push_back(in);
		}
		names.push_back(tmp);
	}

	/// clean from diacritics
	for(auto & author : names)
	{
		for(auto & name : author)
		{
			if(!name.contains('{') && !name.contains('}')) { continue; }
			for(const auto & in : bib::diacritics)
			{
				name.replace(in.first, QChar(in.second));
			}
		}
	}
	return names;
}


QString authorsString(const std::vector<std::vector<QString>> & names,
					  const QString & style)
{
	QString res{};
	int counter = 1;

	for(auto in : names) /// really a copy, not a reference
	{
		QString tmp = style;

		tmp.replace("<L>", in[0]);			/// last name

		tmp.replace("<F>", in[1] + bib::initialsSeparator);			/// full first name
		tmp.replace("<Fs>", in[1][0] + bib::initialsSeparator);		/// short first name


		if(in.size() <= 2)
		{
			tmp.remove("<S>");
			tmp.remove("<Ss>");
		}
		else
		{
			std::function<void(QString&)> func;
			if(tmp.contains("<S>"))
			{
				func = [](QString & inn) { };
			}
			else
			{
				func = [](QString & inn) { inn.resize(1); };
			}

			QString seconds{};
			for(int i = 2; i < in.size(); ++i)
			{
				func(in[i]);
				seconds.push_back(in[i] + bib::initialsSeparator);
			}

			tmp.replace("<S>", seconds);
			tmp.replace("<Ss>", seconds);
		}


		if(counter == bib::authorHowManyEtAl)
		{
			res += "et al" + bib::authorSeparator;
			break;
		}
		res += tmp;
		res += bib::authorSeparator;

		++counter;
	}
	res.chop(authorSeparator.size());
	return res;
}


/// main function
QString Bib::asStyle() const
{
	QString res = this->style;

	/// at first - possible absent attributes
	/// remove square brackets if present, remove all if absent
	QRegularExpression toFind{R"(\[.*?\])"}; /// [ anything ]
	auto mat = toFind.match(res);
	while(mat.hasMatch())
	{
		QString tmp = mat.captured();

		QRegularExpression toFind2{R"(\<[a-zA-Z]+?\>)"}; /// <anything>
		auto mat2 = toFind2.match(tmp); /// we definitely find it

		auto it = std::find_if(std::begin(bib::styleAcronyms),
							   std::end(bib::styleAcronyms),
							   [&mat2](const auto & par)
		{ return par.first == mat2.captured(); });

		QString val = dt.at((*it).second);
		if(!val.isEmpty())
		{
			QString tmpNew = tmp;
			tmpNew.remove('['); tmpNew.remove(']');
			res.replace(tmp, tmpNew);
		}
		else
		{
			res.remove(tmp);
		}
		mat = toFind.match(res);
	}

	/// other attributes
	for(const auto & acr : bib::styleAcronyms)
	{
		if(!res.contains(acr.first)) { continue; }

		if(acr.first == "<Auth>" || acr.first == "<eds>")
		{
			res.replace(acr.first,
						bib::authorsString(
							bib::authorsFromData(this->dt.at(acr.second)),
							bib::authorStyle)
						);
		}
		else if(acr.first == "<Js>")
		{
			if(dt.find(acr.second) != std::end(dt))
			{
				res.replace(acr.first, this->dt.at(acr.second));
			}
			else /// if there's no abbreviation
			{
				res.replace(acr.first, this->dt.at(journal));
			}
		}
		else if(acr.first == "<m>") /// month
		{
			if(dt.find(acr.second) != std::end(dt))
			{
				const QString a = this->dt.at(acr.second);
				res.replace(acr.first, months.at(a).left(3));
			}
			else
			{
				std::cout << "no month but intended" << std::endl;
				res.remove(acr.first);
			}
		}
		else
		{
			res.replace(acr.first, this->dt.at(acr.second));
		}
	}

	/// deal with double dash
	res.replace("--", QString(0x2013));
	/// deal with double dots
	res.replace(QRegExp("\\.{2,}"), ".");

	return res;
}
int Bib::year() const
{
	return dt.at(bib::year).toInt();
}

QString Bib::firstAuthor() const
{
	return this->authors[0][0] + this->authors[0][1][0];
}

bool Bib::isEng() const
{
	return dt.at(bib::lang)[0].toLower() == 'e';
}

bool Bib::isRus() const
{
	return dt.at(bib::lang)[0].toLower() == 'r';
}

bool Bib::isTranslated() const
{
	return dt.at(bib::lang)[0].toLower() == 't';
}

bool Bib::hasAttribute(const QString & attr) const
{
	return dt.find(attr) != std::end(dt);
}

bool Bib::hasAttributesAll(const std::vector<QString> & attrs) const
{
	for(const auto & in : attrs)
	{
		if(!hasAttribute(in)) { return false; }
	}
	return true;
}

bool Bib::hasAttributesOne(const std::vector<QString> & attrs) const
{
	for(const auto & in : attrs)
	{
		if(hasAttribute(in)) { return true; }
	}
	return false;
}

QString Bib::get(const QString & attribute) const
{
	if(!hasAttribute(attribute)) { return {}; }
	return dt.at(attribute);
}


} // end namespace bib
