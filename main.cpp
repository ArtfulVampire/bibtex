#include <QCoreApplication>
#include "lib.h"
#include <QRegularExpression>
#include <QByteArray>

using namespace bib;

int main()
{
	const QString dirPath = "/media/Files/Dropbox/IHNA/PapersWorksBooks"
							 "/OurLabPapers/ForSite";

	const QString allLab = dirPath + "/AllLab/bibtex";

	/// popular remove filters
	auto noDoi = [](const bib::Bib & in) -> bool
	{ return !in.hasAttribute(bib::doi); };

	auto noDoiOrPmid = [](const bib::Bib & in) -> bool
	{ return !in.hasAttributesOne({bib::doi, bib::PMID}); };

	auto isRus = [](const bib::Bib & in) -> bool
	{ return in.isRus(); };

	auto isTr = [](const bib::Bib & in) -> bool
	{ return in.isTranslated(); };



#if 01
	/// make files
	bib::BibTable table(allLab);
	const QStringList guys
	{
		"Martynova",
		"Portnova",
		"Gladun",
		"Atanov",
		"Polishchuk",
		"Liuakovich",
		"Sake",
		"Ivanitsky",
		"Ukraintseva"
	};

//	for(const QString & guy : guys)
	for(const QString & guy : {"Portnova"})
	{
		table.byAuthor(guy)
				.sortByYear()
				.removeIf(noDoi)
				.removeIf(isRus)
				.removeIf(isTr)
				.toFile(dirPath + "/" + guy + ".txt");
	}
#endif




#if 0
	/// clean AllLab/bibtex

#if 0
	for(const QString & fn : QDir(allLab).entryList({"*.txt"}))
	{
		{
			/// clean of prepended numbers
			QFile::rename(allLab + "/" + fn,
						  allLab + "/" + fn.mid(fn.indexOf('_') + 1));
		}
	}
#endif

#if 0
		/// check titles duplicate
	std::map<QString, int> mp{};
	for(const QString & fn : QDir(allLab).entryList({"*.txt"}))
	{
		bib::Bib bb(allLab + "/" + fn);
		if(mp[bb.get(bib::title)] == 1) { std::cout << bb.getFileName() << std::endl; }
		mp[bb.get(bib::title)]= 1;
	}

	for(auto in = mp.begin(); in != mp.end(); ++in)
	{
		if(in->second != 1)
		{
//			std::cout << in->first << std::endl;
		}
	}

#endif

#if 0
	/// check lang field
	for(const QString & fn : QDir(allLab).entryList({"*.txt"}))
	{
		/// check titles duplicate
		bib::Bib bb(allLab + "/" + fn);
		if(!bb.hasAttribute(bib::lang)) { std::cout << bb.getFileName() << std::endl; }
	}
#endif

#if 0
	/// check Spacial
	for(const QString & fn : QDir(allLab).entryList({"*.txt"}))
	{
		/// check titles duplicate
		bib::Bib bb(allLab + "/" + fn);
		if(bb.get(bib::title).contains("Spacial")) { std::cout << bb.getFileName() << std::endl; }
	}
#endif

	exit(0);
#endif



#if 0
	if(0)
	{
		/// make short medline - done
		QFile medIn("/home/michael/Projects/bibtex/medline.txt");
		medIn.open(QIODevice::ReadOnly);
		QByteArray medCont;
		while(!medIn.atEnd())
		{
			QString tmp = medIn.readLine();
			if(!tmp.startsWith("JournalTitle") && !tmp.startsWith("MedAbbr"))
			{
				continue;
			}
			else
			{
				medCont.append(tmp);
			}
		}
		medCont.replace("\nMedAbbr: ", "_");
		medCont.replace("JournalTitle: ", "");

		QFile medOut("/home/michael/Projects/bibtex/medlineShort.txt");
		medOut.open(QIODevice::WriteOnly);
		medOut.write(medCont);
		medOut.close();
	}
#endif

#if 0
	/// deal with duplicates in medlineShort.txt - done
	QFile medIn("/home/michael/Projects/bibtex/medlineShort.txt");
	medIn.open(QIODevice::ReadOnly);
	QByteArray medCont;
	while(!medIn.atEnd())
	{
		QString tmp = medIn.readLine();
		if(tmp.contains('='))
		{
			auto lst = tmp.split(QRegExp(R"(( \= )|_)"), QString::SkipEmptyParts);
			for(int i = 0; i < lst.size() - 1; ++i)
			{
				medCont.append(lst[i] + "_" + lst[lst.size() - 1]);
			}
//			for(const auto & in : lst)
//			{
//				std::cout << in << std::endl;
//			}
//			std::cout << std::endl;

		}
		else
		{
			medCont.append(tmp);
		}
	}

	QFile medOut("/home/michael/Projects/bibtex/medlineShortDup.txt");
	medOut.open(QIODevice::WriteOnly);
	medOut.write(medCont);
	medOut.close();

	exit(0);
#endif


	return 0;
}
