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
		std::cout << journal << std::endl;

		/// replace Upper to lower but not in abbreviations and not the first one
		QRegularExpression upp(R"((.+?)([A-Z])(?=[a-z]))");
		auto uppMat = upp.match(journal);
		while(uppMat.hasMatch())
		{
			journal[uppMat.capturedStart(2)] = journal[uppMat.capturedStart(2)].toLower();
			uppMat = upp.match(journal, uppMat.capturedEnd(2));
		}

//		std::cout << std::endl;
		std::cout << journal << std::endl;

		/// find abbr in medline base
		int a = medlineBase.indexOf(journal);
		std::cout << a << std::endl;

		/// if no such journal
		if(a == -1)
		{

			std::cout << std::endl;
			continue;
		}

		int sta = a + journal.size() + 1;
		int len = medlineBase.indexOf('\n', sta) - sta;
		QString abbr = medlineBase.mid(sta, len); /// +1 for _ separator

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
	std::vector<std::vector<QString>> names{}; /// [authName][last/first/second(s)]
	for(const QString & in : lst)
	{
		/// many problems here
		auto l = in.split(QRegExp("[\\s\\,]"), QString::SkipEmptyParts);

		QString first{};
		QString last{};
		QString second{};

		int secondStart{};
		int secondEnd{};

		if(in.contains(',')) /// Last, First Second
		{
			last = l[0];
			first = l[1];
			if(l.size() > 2)
			{
				secondStart = 2;
				secondEnd = l.size();
			}

		}
		else /// First Second(s) Last
		{
			last = l.last();
			first = l.first();
			if(l.size() > 2)
			{
				secondStart = 1;
				secondEnd = l.size() - 1;
			}
		}
		if(l.size() > 2)
		{
			second.clear();
			for(int i = secondStart; i < secondEnd; ++i)
			{
				second += l[i] + " ";
			}
			if(!second.isEmpty()) { second.chop(1); }
		}
		names.push_back({last, first, second});
	}

	QString res{};
	int counter = 1;
	for(auto in : names)
	{
		QString tmp = style;
		/// last name
		tmp.replace("<L>", in[0]);

		/// first name
		tmp.replace("<F>", in[1]);
		in[1].remove(QRegExp("[^A-Z]"));
		tmp.replace("<Fs>", in[1]);

		 /// second name - ok if empty
		tmp.replace("<S>", in[2]);
		in[2].remove(QRegExp("[^A-Z]"));
		tmp.replace("<Ss>", in[2]);

		if(counter == bib::authorHowManyEtAl)
		{
			res += "et al." + bib::authorSeparator;
			break;
		}
		res += tmp;
		res += bib::authorSeparator;

		++counter;
	}
	res.chop(authorSeparator.size());
	return res;
}

Bib::Bib(const QString & bibContents)
{
	QString toParse = bibContents;

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

QString Bib::asStyle(const QString & style)
{
	QString res = style;
	QRegularExpression toFind{R"(\[.*?\])"}; /// [ anything ]
//	int offset = 0;
	auto mat = toFind.match(res);
	while(mat.hasMatch())
	{
		QString tmp = mat.captured();
//		std::cout << tmp << std::endl;

		QRegularExpression toFind2{R"(\<[A-Z]\>)"};
		auto mat2 = toFind2.match(tmp); /// we definitely find it

//		std::cout << mat2.captured() << std::endl;

		auto it = std::find_if(std::begin(bib::styleAcronyms),
							   std::end(bib::styleAcronyms),
							   [&mat2](const auto & par)
		{ return par.first == mat2.captured(); });

		QString val = dt[(*it).second];
//		std::cout << val << std::endl << std::endl;
		if(!val.isEmpty())
		{
			QString tmpNew = tmp;
			tmpNew.replace(mat2.captured(), val);
			tmpNew.remove('['); tmpNew.remove(']');
			res.replace(tmp, tmpNew);
		}
		else
		{
			res.remove(tmp);
		}
		mat = toFind.match(res);
	}


	for(const auto & acr : bib::styleAcronyms)
	{
		if(acr.first == "<A>")
		{
			res.replace(acr.first, bib::authorsFromData(this->dt[acr.second]));
		}
		else if(acr.first == "<j>")
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
		else
		{
			res.replace(acr.first, this->dt[acr.second]);
		}
	}
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

std::vector<QString> BibBase::asStyle(const QString & style)
{
	std::vector<QString> res{};
//	int count = 1;
	for(Bib & in : this->bibs)
	{
//		std::cout << count << std::endl;
		res.push_back(in.asStyle(style));
//		std::cout << count << std::endl;
//		++count;
	}
	return res;
}


} // end namespace bib