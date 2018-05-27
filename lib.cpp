#include "lib.h"

#include <QRegularExpression>

namespace bib
{

/// deprecate
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
				const QStringList & filters)
{
	QFile medFile("/home/michael/Projects/bibtex/medlineShortDup.txt");
	medFile.open(QIODevice::ReadOnly);
	auto medlineBase = medFile.readAll();
	medFile.close();

	for(const auto & fileName : QDir(filesDir).entryList(filters))
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

void addSquareBrackets(const QString & filesDir,
					   const QStringList & filters)
{

	for(const QString & fn : QDir(filesDir).entryList(filters))
	{
		QFile fil(filesDir + "/" + fn);
		fil.open(QIODevice::ReadOnly);
		auto contents = fil.readAll();
		fil.close();

		/// check if translated
		if(!contents.contains("lang={t") && !contents.contains("lang={T")) { continue; }

		QRegularExpression toFind(R"((.*title\=\{)(.*)(\}\,))");
		auto mat = toFind.match(contents);
		const QString oldTitle = mat.captured(2);
		QString newTitle = oldTitle;
		bool rewrite{false};
		if(!newTitle.startsWith("["))
		{
			newTitle.prepend("[");
			rewrite = true;
		}
		if(!newTitle.endsWith("]"))
		{
			newTitle += "]";
			rewrite = true;
		}

		if(rewrite)
		{
			contents.replace(oldTitle, newTitle.toUtf8());
			fil.open(QIODevice::WriteOnly);
			fil.write(contents);
			fil.close();
		}
	}
}


void correctZhVND(const QString & filesDir,
				  const QStringList & filters)
{
	for(const QString & fn : QDir(filesDir).entryList(filters))
	{
		QFile fil(filesDir + "/" + fn);
		fil.open(QIODevice::ReadOnly);
		auto contents = fil.readAll();
		fil.close();

		/// check if ZhVND
		if(!contents.contains("journal={Zhurnal vysshei nervnoi")) { continue; }
		const QString newJournal = "Zhurnal vysshei nervnoi deiatelnosti imeni I P Pavlova";

		QRegularExpression toFind(R"((.*journal\=\{)(.*)(\}\,))");
		auto mat = toFind.match(contents);
		const QString oldJournal = mat.captured(2);

		if(newJournal != oldJournal)
		{
			contents.replace(oldJournal, newJournal.toUtf8());
			fil.open(QIODevice::WriteOnly);
			fil.write(contents);
			fil.close();
		}
	}
}



} // end namespace bib
