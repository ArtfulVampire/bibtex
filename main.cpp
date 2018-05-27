#include <QCoreApplication>
#include "lib.h"
#include <QRegularExpression>
#include <QByteArray>

using namespace bib;

int main()
{
	const QString dirPath = "/media/Files/Dropbox/IHNA/PapersWorksBooks"
							 "/OurLabPapers/ForSite";

	bib::BibTable table(dirPath + "/AllLab/bibtex");
	const QStringList guys = QDir(dirPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);

//	for(const QString & guy : guys)
	for(const QString & guy : {"Portnova"})
	{
		table.byAuthor(guy).sortByYear().toFile(dirPath + "/" + guy + ".txt");
	}




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
