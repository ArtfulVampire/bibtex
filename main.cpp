#include <QCoreApplication>
#include "lib.h"
#include <QRegularExpression>
#include <QByteArray>

using namespace bib;

int main()
{
	const QString workPath = "/media/Files/Dropbox/IHNA/PapersWorksBooks/OurLabPapers/PapersToPublish/GalyaTBIspaces/bibtex";
	const QStringList filters = {"*.txt"};



	if(0)
	{
		/// add medAbbr to files
		QFile medFile("/home/michael/Projects/bibtex/medlineShort.txt");
		medFile.open(QIODevice::ReadOnly);
		auto medCont = medFile.readAll();
		medFile.close();

		bib::addMedAbbr(workPath + "/txts/test",
						filters,
						medCont);
	}


	if(0)
	{
		/// make short medline
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


	if(0)
	{
		/// make base

		bib::manyFilesToOne(workPath + "/txts/test",
							filters,
							workPath + "/baseab.bib");
	}
	if(01)
	{
		/// out to reference.txt

		/// read base
		QFile inFile(workPath + "/baseab.bib");
		inFile.open(QIODevice::ReadOnly);
		QString contents = inFile.readAll();
		inFile.close();
		bib::BibBase bas(contents);

		/// output
		QFile outFile(workPath + "/reference.txt");
		outFile.open(QIODevice::WriteOnly);
		for(const QString & in : bas.asStyle())
		{
			outFile.write(QByteArray((in + "\n").toStdString().c_str()));
		}
		outFile.close();
	}


	return 0;
}
