#include <QCoreApplication>
#include "lib.h"
#include <QRegularExpression>
#include <QByteArray>

using namespace bib;

int main()
{
	const QStringList filters = {"*.txt"};
	const QString workPath = "/media/Files/Dropbox/IHNA/PapersWorksBooks"
							 "/OurLabPapers/MyPapers/bibtex/eng";

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


	if(0)
	{
		/// add medAbbr to files
		QFile medFile("/home/michael/Projects/bibtex/medlineShortDup.txt");
		medFile.open(QIODevice::ReadOnly);
		auto medCont = medFile.readAll();
		medFile.close();

		bib::addMedAbbr(workPath,
						filters,
						medCont);
	}

	if(01)
	{
		/// make base

		bib::manyFilesToOne(workPath,
							filters,
							workPath + "/base");

		/// out to reference.txt
		/// read base
		QFile inFile(workPath + "/base");
		inFile.open(QIODevice::ReadOnly);
		QString contents = inFile.readAll();
		inFile.close();
		bib::BibBase bas(contents);


		/// output
		QFile outFile(workPath + "/reference");
		outFile.open(QIODevice::WriteOnly);
		for(const QString & in : bas.asStyle())
		{
			outFile.write(QByteArray((in + "\n\n").toStdString().c_str()));
		}
		outFile.close();
	}


	return 0;
}
