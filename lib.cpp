#include "lib.h"

#include <QRegularExpression>

namespace bib
{

void manyFilesToOne(const QString & workPath,
					const QStringList & filters,
					const QString outFilePath)
{
	/// bibtex database from files
	QFile outFile(outFilePath);
	outFile.open(QIODevice::WriteOnly);

	for(const QString & bib : QDir(workPath).entryList(filters))
	{
		QFile fil(workPath + "/" + bib);
		fil.open(QIODevice::ReadOnly);
		auto contents = fil.readAll();
		outFile.write(contents);
		outFile.write("\n\n");
		fil.close();
	}
	outFile.close();
}

void addMedAbbr(const QString & filesDir,
				const QStringList & filters,
				const QByteArray & medlineBase)
{
	auto lst = QDir(filesDir).entryList(filters);
	for(const auto & fileName : lst)
	{
		/// read file
		QString filePath = filesDir + "/" + fileName;
		QFile oneBib(filePath);
		oneBib.open(QIODevice::ReadOnly);
		QString oneBibCont = oneBib.readAll();
		oneBib.close();

		if(oneBibCont.contains(QRegExp(R"(\s*abbr\=\{.*\}\,?\s*)"))) { continue; }

		/// find journal name
		QRegularExpression toFind{R"((\s*journal\=\{)(.*)(\}\,?))"};
		auto mat = toFind.match(oneBibCont);
		if(!mat.hasMatch()) { continue; }

		QString journal = mat.captured(2);
		journal.remove('\\');
		std::cout << journal << std::endl; /// read journal name

		/// replace Upper to lower but not in abbreviations and not the first one
		/// ??????
		QRegularExpression upp(R"((.+?)([A-Z])(?=[a-z]))");
		auto uppMat = upp.match(journal);
		while(uppMat.hasMatch())
		{
			/// why 2?
			journal[uppMat.capturedStart(2)] = journal[uppMat.capturedStart(2)].toLower();
			uppMat = upp.match(journal, uppMat.capturedEnd(2));
		}
		std::cout << journal << std::endl; /// to lower

		/// find abbr in medline base
		int a = medlineBase.indexOf(journal);
		std::cout << a << std::endl;

		/// if no such journal
		if(a == -1)
		{
			std::cout << std::endl;
			continue;
		}

		int sta = medlineBase.indexOf('_', a + journal.size() - 1) + 1; /// start of the abbr
		int len = medlineBase.indexOf('\n', sta) - sta;
		QString abbr = medlineBase.mid(sta, len);

		/// no abbrev given
		if(abbr.size() < 3)
		{
			std::cout << std::endl;
			continue;
		}

		std::cout << abbr << std::endl;
		std::cout << std::endl;

		/// insert abbr
		int ins = oneBibCont.indexOf("\n", mat.capturedStart() + 1);
		oneBibCont.insert(ins + 1, "  abbr={" + abbr + "},\n");

		/// write new
		oneBib.open(QIODevice::WriteOnly);
		oneBib.write(oneBibCont.toStdString().c_str());
		oneBib.close();
	}
}

QString authorsFromData(const QString & authors, const QString & style)
{
	QStringList lst = authors.split(" and ", QString::SkipEmptyParts);
	std::vector<std::vector<QString>> names{}; /// [authNum][last/first/second(s)]

	for(const QString & in : lst)
	{
		/// many problems here

		QString first{};
		QString last{};
		QString second{};

		int secondStart{};
		int secondEnd{};

		if(in.contains(',')) /// Last, First Second - usual format
		{
			last = in.left(in.indexOf(','));
			QString in2 = in.mid(in.indexOf(','));	/// cut last name

			auto ll = in2.split(QRegExp(R"([, ])"), QString::SkipEmptyParts);

			/// double letters - first and second names.
			/// what with cyrillic???
			if(ll.size() == 1 && ll[0].size() == 2 && ll[0][0].isUpper() && ll[0][1].isUpper())
			{
				first = ll[0][0];
				second = ll[0][1];
			}
			else
			{
				first = ll[0];
				if(ll.size() > 1)
				{
					secondStart = 1;
					secondEnd = ll.size();

					second.clear();
					for(int i = secondStart; i < secondEnd; ++i)
					{
						second += ll[i] + " ";
					}
					if(!second.isEmpty()) { second.chop(1); }
				}
			}

		}
		else /// First Second(s) Last
		{
			auto l = in.split(QRegExp(R"([, ])"), QString::SkipEmptyParts);
			last = l.last();
			first = l.first();
			if(l.size() > 2)
			{
				secondStart = 1;
				secondEnd = l.size() - 1;
				second.clear();
				for(int i = secondStart; i < secondEnd; ++i)
				{
					second += l[i] + " ";
				}
				if(!second.isEmpty()) { second.chop(1); }
			}
		}
		names.push_back({last, first, second});
	}

	/// clean from diacritics
	for(auto & in1 : names)
	{
		for(auto & in2 : in1)
		{
			for(const auto & in : bib::diacritics)
			{
				in2.replace(in.first, QChar(in.second));
			}
		}
	}

	QString res{};
	int counter = 1;
	for(auto in : names) /// really a copy, not a reference
	{
		QString tmp = style;
		/// last name
		tmp.replace("<L>", in[0]);

		/// first name
		/// full
		tmp.replace("<F>", in[1]);
		/// short
//		in[1].remove(QRegExp("[^A-Z]"));
		in[1].resize(1);
		tmp.replace("<Fs>", in[1]);

		/// second name
		if(!in[2].isEmpty())
		{
			tmp.replace("<S>", in[2]);
//			in[2].remove(QRegExp("[^A-Z]"));
			in[2].resize(1);
			tmp.replace("<Ss>", in[2]);
		}
		else
		{
			tmp.remove("<S>");
			tmp.remove("<Ss>");
		}

		/// double dots are dealt in the end of asStyle()

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

const std::map<QString, QString> Bib::months
{
	{"01",	"January"},
	{"1",	"January"},
	{"02",	"February"},
	{"2",	"February"},
	{"03",	"March"},
	{"3",	"March"},
	{"04",	"April"},
	{"4",	"April"},
	{"05",	"May"},
	{"5",	"May"},
	{"06",	"June"},
	{"6",	"June"},
	{"07",	"July"},
	{"7",	"July"},
	{"08",	"August"},
	{"8",	"August"},
	{"09",	"September"},
	{"9",	"September"},
	{"10",	"October"},
	{"11",	"November"},
	{"12",	"December"}
};

Bib::Bib(const QString & bibContents)
{
	QString toParse = bibContents;

	/// set style
	if(bibContents.startsWith("article"))
	{
		this->format = Style::article;
		this->style = bib::articleStyle;
	}
	else if(bibContents.startsWith("book"))
	{
		this->format = Style::book;
		this->style = bib::bookStyle;
	}
	else if(bibContents.startsWith("conference"))
	{
		this->format = Style::conference;
		this->style = bib::confStyle;
	}
	else if(bibContents.startsWith("inproceedings"))
	{
		this->format = Style::proceedings;
		this->style = bib::procStyle;
	}
	else if(bibContents.startsWith("chapter"))
	{
		this->format = Style::chapter;
		this->style = bib::chapterStyle;
	}
	else
	{
		this->format = Style::unknown;
		this->style = bib::unknownStyle;
	}

//	std::cout << this->style << std::endl;

	/// chop last '}'
	toParse.resize(toParse.lastIndexOf('}'));

	/// chop first ...{
	toParse = toParse.right(toParse.size() - toParse.indexOf('{'));


	for(const auto & acr : bib::styleAcronyms)
	{
		QRegularExpression toFind{"(?<=" + acr.second + R"(\=\{)(.*)(?=\}))"};
		auto mat = toFind.match(toParse);
		if(mat.hasMatch())
		{
			this->dt[acr.second] = mat.captured();
		}
	}
	/// chop last dot in title
	if(dt["title"].endsWith('.')) { dt["title"].chop(1); }
}

/// main function
QString Bib::asStyle()
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

		QString val = dt[(*it).second];
		if(!val.isEmpty())
		{
			QString tmpNew = tmp;
//			tmpNew.replace(mat2.captured(), val);
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
			res.replace(acr.first, bib::authorsFromData(this->dt[acr.second]));
		}
		else if(acr.first == "<Js>")
		{
			if(!this->dt[acr.second].isEmpty())
			{
				res.replace(acr.first, this->dt[acr.second]);
			}
			else /// if there's no abbreviation
			{
				res.replace(acr.first, this->dt["journal"]);
			}
		}
		else if(acr.first == "<m>") /// month
		{
			const QString a = this->dt[acr.second];
			res.replace(acr.first, months.at(a).left(3));
		}
		else
		{
			res.replace(acr.first, this->dt[acr.second]);
		}
	}

	/// deal with double dash
	res.replace("--", QString(0x2013));
	/// deal with double dots
	res.replace(QRegExp("\\.{2,}"), ".");

	return res;
}
int Bib::year()
{
	return dt["year"].toInt();
}

QString Bib::firstAuthor()
{
	QString tmp = dt["authors"];
	return tmp.left(tmp.indexOf(QRegExp("\\W")));
}

BibBase::BibBase(const QString & baseContents)
{
	QRegularExpression toFind{R"((?<=@)(.*?\})(?=\,?\n\t*\n))"};
	toFind.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);
	int offset = 0;
	auto mat = toFind.match(baseContents);
	while(mat.hasMatch())
	{
		QString tmp = mat.captured();
//		std::cout << tmp << std::endl;
		this->bibs.push_back(bib::Bib(tmp));
		offset += tmp.size();
		mat = toFind.match(baseContents, offset);
	}

}

std::vector<QString> BibBase::asStyle()
{
	std::vector<QString> res{};
//	int count = 1;
//	std::cout << "asdasd" << std::endl;
	for(Bib & in : this->bibs)
	{
//		std::cout << count << std::endl;
		res.push_back(in.asStyle());
//		std::cout << count << std::endl;
//		++count;
	}
	return res;
}


} // end namespace bib