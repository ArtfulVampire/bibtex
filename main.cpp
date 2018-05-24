#include <QCoreApplication>
#include "lib.h"
#include <QRegularExpression>
#include <QByteArray>

using namespace bib;

int main()
{
	const QStringList filters = {"*.txt"};


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

	const QString dirPath = "/media/Files/Dropbox/IHNA/PapersWorksBooks"
							 "/OurLabPapers/ForSite";
	const QStringList guys = QDir(dirPath).entryList(QDir::Dirs | QDir::NoDotAndDotDot);

//	QFile medFile("/home/michael/Projects/bibtex/medlineShortDup.txt");
//	medFile.open(QIODevice::ReadOnly);
//	auto medCont = medFile.readAll();
//	medFile.close();


	for(const QString & guy : guys)
	{
//		if(guy == "AllLab" || guy == "Atanov" || guy == "Balaev") { continue; }
		QString workPath = dirPath + "/" + guy + "/bibtex";

#if 01

		bib::manyFilesToOne(workPath,
							filters,
							workPath + "/base");

		/// read file
		QFile inFile(workPath + "/base");
		inFile.open(QIODevice::ReadOnly);
		QString contents = inFile.readAll();
		inFile.close();

		/// make Base
		bib::BibBase bas(contents);

		/// print asStyle
		QFile outFile(dirPath + "/" + guy + ".txt");
		outFile.open(QIODevice::WriteOnly);
		for(const QString & in : bas.asStyle())
		{
			outFile.write(QByteArray((in + "\n\n").toStdString().c_str()));
		}
		outFile.close();
#endif

#if 0
		bib::addMedAbbr(workPath,
						filters,
						medCont);
#endif

#if 0
		if(01)
		{
			const QStringList bibs = QDir(workPath).entryList({"*.txt"});
			for(const QString fn : bibs)
			{
				QFile fil(workPath + "/" + fn);
				fil.open(QIODevice::ReadOnly);
				auto contents = fil.readAll();
				fil.close();

				/// correct ZhVND
				if(0)
				{
					QRegularExpression toFind(R"((.*journal\=\{)(.*)(\}\,))");
					auto mat = toFind.match(contents);
					const QString oldJournal = mat.captured(2);
					QString newJournal = oldJournal;
					if(newJournal.contains("Zhurnal vysshei nervnoi"))
					{
						newJournal = "Zhurnal vysshei nervnoi deiatelnosti imeni I P Pavlova";
					}
					contents.replace(oldJournal, newJournal.toUtf8());
				}


				/// square brackets
				if(0)
				{
					QRegularExpression toFind(R"((.*title\=\{)(.*)(\}\,))");
					auto mat = toFind.match(contents);
					const QString oldTitle = mat.captured(2);
//					std::cout << guy << "\t" << fn << std::endl;
//					std::cout << oldTitle << std::endl << std::endl;;
					QString newTitle = oldTitle;
					if(!newTitle.startsWith("["))
					{
						newTitle = "[" + newTitle;
					}
					if(!newTitle.endsWith("]"))
					{
						newTitle = newTitle + "]";
					}
					contents.replace(oldTitle, newTitle.toUtf8());
				}
				if(contents.size() > 1000 && 0)
				{
					std::cout << guy << "\t" << fn << std::endl;
					std::cout << contents.toStdString() << std::endl << std::endl;
				}

				if(!contents.contains("doi={")
				   && !contents.contains("PMID={")
				   && !contents.contains("url={")
				   && 0
				   )
				{
					std::cout << guy << "\t" << fn << "\t" << "no DOI and PMID" << std::endl;
				}

				if(contents.contains("@book")
				   && 0
				   )
				{
					std::cout << guy << "\t" << fn << "\t" << "no DOI and PMID" << std::endl;
				}

//				contents.replace("date={", "day={");


//				fil.open(QIODevice::WriteOnly);
//				fil.write(contents);
//				fil.close();
			}
		}
#endif
	}
	exit(0);

#if 0
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
#endif


	return 0;
}
