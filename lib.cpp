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

QString authorsFromData(const QString & authors, const QString & style)
{
	QStringList lst = authors.split("and", QString::SkipEmptyParts);
	std::vector<std::vector<QString>> names{}; /// [authName][first/second/last]
	for(const QString & in : lst)
	{
		/// many problems here
		auto l = in.split(QRegExp("[\\s\\,]"), QString::SkipEmptyParts);

		if(l.size() == 3)
		{
			if(in.contains(',')) /// Last, First Second
			{
				names.push_back({l[1], l[2], l[0]});
			}
			else /// First Second Last
			{
				names.push_back({l[0], l[1], l[2]});
			}
		}
		else /// if(l.size() == 2)
		{
			if(in.contains(',')) /// Last, First
			{
				names.push_back({l[1], l[0]});
			}
			else /// First Last
			{
				names.push_back({l[0], l[1]});
			}
		}
	}

	QString res{};
	for(auto in : names)
	{
		QString tmp = style;
		if(in.size() == 3)
		{
			tmp.replace("<S>", in[1]);
			in[1].remove(QRegExp("[a-z]"));
			tmp.replace("<Ss>", in[1]);

			tmp.replace("<F>", in[0]);
			in[0].remove(QRegExp("[a-z]"));
			tmp.replace("<Fs>", in[0]);

			tmp.replace("<L>", in[2]);
		}
		else /// if(l.size() == 2) /// without second name
		{
//			tmp.replace("<Ss>", in[1][0]);
//			tmp.replace("<S>", in[1]);
			tmp.remove("<S>");
			tmp.remove("<Ss>");

			tmp.replace("<F>", in[0]);
			in[0].remove(QRegExp("[a-z]"));
			tmp.replace("<Fs>", in[0]);

			tmp.replace("<L>", in[1]);
		}

		res += tmp;
		res += bib::authorSeparator;
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
	toParse = toParse.right(toParse.size() - toParse.indexOf('{') + 1);

//	std::cout << toParse << std::endl;

	for(const auto & acr : bib::styleAcronyms)
	{
		QRegularExpression toFind{R"((?<=)" + acr.second + R"(\=\{)(.*)(?=\}))"};
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
//		std::cout << val << '_' << std::endl;
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
		if(acr.first != "<A>")
		{
			res.replace(acr.first, this->dt[acr.second]);
		}
		else
		{
			res.replace(acr.first, bib::authorsFromData(this->dt[acr.second]));
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

	QRegularExpression toFind{R"((?<=@)(.*?)(?=(\}\,{0,1})\n\s*?\n))"};
	toFind.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);
	int offset = 0;
	auto mat = toFind.match(baseContents);
	while(mat.hasMatch())
	{
		this->bibs.push_back(bib::Bib(mat.captured()));
		offset += mat.captured().size();
		mat = toFind.match(baseContents, offset);
	}

}

std::vector<QString> BibBase::asStyle(const QString & style)
{
	std::vector<QString> res{};
	for(Bib & in : this->bibs)
	{
		res.push_back(in.asStyle(style));
	}
	return res;
}


} // end namespace bib