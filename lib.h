#ifndef LIB_H
#define LIB_H

#include <QChar>
#include <QString>
#include <QFile>
#include <QDir>

#include <map>
#include <iostream>
#include <ios>
#include <utility>

namespace bib
{

inline std::ostream & operator<< (std::ostream & os, const QString & toOut)
{
	os << toOut.toStdString();
	return os;
}


const std::vector<std::pair<QString, QString>> styleAcronyms
{
	{"<Auth>", "author"},
	{"<Title>", "title"},
	{"<B>", "booktitle"},
	{"<y>", "year"},
	{"<d>", "day"},
	{"<m>", "month"},
	{"<J>", "journal"},
	{"<Js>", "abbr"},
	{"<Vol>", "volume"},
	{"<Num>", "number"},
	{"<Pag>", "pages"},
	{"<Pub>", "publisher"},
	{"<Org>", "organization"},
	{"<eds>", "editors"},
	{"<City>", "city"},
	{"<Co>", "country"},
	{"<St>", "state"},
	{"<DOI>", "doi"},
	{"<PMID>", "PMID"},
	{"<ISBN>", "ISBN"}
};



/// make [<Ss>.]
const QString authorStyle = "<L> <Fs>.<Ss>."; /// L-last name, F - first name, S - second name, s - short
const QString authorSeparator = ", ";
const int authorHowManyEtAl = 11;

/// make alternative condition {ifhas1|ifhas2|ifhas3}, stop at first occurence
#if 0
/// Taylor and Francis - https://www.tandfonline.com/action/authorSubmission?journalCode=ibij20&page=instructions
const QString articleStyle = "<Auth> <Title>. <J>. <y>[; <Vol>][(<Num>)][: <Pag>].[ https://doi.org/<DOI>]";
const QString chapterStyle = "<Auth>. <Title>. pp. <Pag> In: <B>, <Pub>, eds. <eds>, <City>, <y>.[ ISBN <ISBN>]";
const QString bookStyle = "<Auth>. <B>. <Pub>, eds. <eds>, <City> <y>.[ ISBN <ISBN>]";
const QString confStyle = "<Auth>. <Title>. Paper presented at: <Conf>; <y> <m> <d>; <City>, <Co>.";
const QString unknownStyle = "<Auth> <Title> [<J>][<B>]. <y>[ https://doi.org/<DOI>]";
#elif 1
/// IHNA for site
const QString articleStyle = "<Auth> <Title>. <J>. <y>[; <Vol>][(<Num>)][: <Pag>].[ PMID: <PMID>][ https://doi.org/<DOI>]";
const QString chapterStyle = "<Auth>. <Title>.[ pp. <Pag>] In: <B>, <y>. <Pub>[, eds. <eds>][, <City>].[ ISBN <ISBN>]";
const QString bookStyle = "<Auth> <B>. <Pub>, eds. <eds>, <City> <y>.[ ISBN <ISBN>]";
const QString confStyle = "<Auth> <Title>. At: <Conf>; <y> <m> <d>; <City>, <Co>."; /// oral conference
const QString procStyle = "<Auth> <Title>. In: <B>, <y>.[ <Pub>][, eds. <eds>.][ <d>][ <m>][, <City>][, <Co>].[ https://doi.org/<DOI>]"; /// written proceedings
const QString unknownStyle = "<Auth> <Title> [<J>][<B>]. <y>[ https://doi.org/<DOI>]";
#else

#endif

enum class Style {unknown, article, book, proceedings, conference, chapter};

class Bib
{
public:
	static const std::map<QString, QString> months;
	QString asStyle();
	int year();
	QString firstAuthor();

	Bib(const QString & bibContents);

private:
	std::map<QString, QString> dt{};
	Style format{};
	QString style{};
};

class BibBase
{
private:
	std::vector<Bib> bibs;

public:
	BibBase(const QString & baseContents);
	std::vector<QString> asStyle();
};




void manyFilesToOne(const QString & workPath,
					const QStringList & filters,
					const QString outFilePath);

void addMedAbbr(const QString & filesDir,
				const QStringList & filters,
				const QByteArray & medlineBase);

QString authorsFromData(const QString & authors, const QString & style = authorStyle);


const std::map<QString, int> diacritics{
	/// a
	{"{\\`a}", 224},
	{"{\\'a}", 225},
	{"{\\^a}", 226},
	{"{\\~a}", 227},
	{"{\\\"a}", 228},

	{"{\\`A}", 192},
	{"{\\'A}", 193},
	{"{\\^A}", 194},
	{"{\\~A}", 195},
	{"{\\\"A}", 196},
	/// e
	{"{\\`e}", 232},
	{"{\\'e}", 233},
	{"{\\^e}", 234},
	{"{\\\"e}", 235},
	{"{\\ue}", 277},
	{"{\\ve}", 283},

	{"{\\`E}", 200},
	{"{\\'E}", 201},
	{"{\\^E}", 202},
	{"{\\\"E}", 203},
	{"{\\uE}", 276},
	{"{\\vE}", 282},
	/// o
	{"{\\`o}", 242},
	{"{\\'o}", 243},
	{"{\\^o}", 244},
	{"{\\~o}", 245},
	{"{\\\"o}", 246},
	{"{\\o}",  248},

	{"{\\`O}", 210},
	{"{\\'O}", 211},
	{"{\\^O}", 212},
	{"{\\~O}", 213},
	{"{\\\"O}", 214},
	{"{\\O}",  216},
	/// u
	{"{\\`u}", 249},
	{"{\\'u}", 250},
	{"{\\^u}", 251},
	{"{\\\"u}", 252},

	{"{\\`U}", 217},
	{"{\\'U}", 218},
	{"{\\^U}", 219},
	{"{\\\"U}", 220},
	
	/// i
	{"{\\u\\i}}", 0x12d}, /// with brevis

	/// consonants
	{"{\\cc}", 231},
	{"{\\vc}", 269},

	{"{\\cC}", 199},
	{"{\\vC}", 268},

	{"{\\vr}", 345},
	{"{\\vR}", 344},
	{"{\\vz}", 382},
	{"{\\vZ}", 381}
};

} // end namespace bib

#endif // LIB_H