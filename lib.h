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

/// to replace
const QString author		= "author";
const QString title			= "title";
const QString booktitle		= "booktitle";
const QString year			= "year";
const QString day			= "day";
const QString month			= "month";
const QString journal		= "journal";
const QString abbr			= "abbr";
const QString volume		= "volume";
const QString number		= "number";
const QString pages			= "pages";
const QString publisher		= "publisher";
const QString organization	= "organization";
const QString editors		= "editors";
const QString city			= "city";
const QString country		= "country";
const QString state			= "state";
const QString doi			= "doi";
const QString PMID			= "PMID";
const QString ISBN			= "ISBN";
const QString lang			= "lang";			/// ru/rus, en/eng, translated title


const QString article			= "article";
const QString book				= "book";
const QString conference		= "conference";
const QString inproceedings		= "inproceedings";
const QString chapter			= "chapter";

const std::vector<std::pair<QString, QString>> styleAcronyms
{
	{"<Auth>",	author},
	{"<Title>",	title},
	{"<B>",		booktitle},
	{"<y>",		year},
	{"<d>",		day},
	{"<m>",		month},
	{"<J>",		journal},
	{"<Js>",	abbr},
	{"<Vol>",	volume},
	{"<Num>",	number},
	{"<Pag>",	pages},
	{"<Pub>",	publisher},
	{"<Org>",	organization},
	{"<eds>",	editors},
	{"<City>",	city},
	{"<Co>",	country},
	{"<St>",	state},
	{"<DOI>",	doi},
	{"<PMID>",	PMID},
	{"<ISBN>",	ISBN}
};



/// make [<Ss>.] ?
const QString authorStyle = "<L> <Fs><Ss>"; /// L-last name, F - first name, S - second name, s - short
const QString authorSeparator = ", ";
const QString initialsSeparator = ".";
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

/// vector of {Last, First, Second(s)}
std::vector<std::vector<QString>> authorsFromData(const QString & authors);
QString authorsString(const std::vector<std::vector<QString>> & names,
					  const QString & style);
class Bib
{
public:
	QString asStyle() const;
	int year() const;
	QString firstAuthor() const;
	bool isEng() const;
	bool isRus() const;
	bool isTranslated() const;
	bool hasAttribute(const QString & attr) const;
	bool hasAttributesAll(const std::vector<QString> & attrs) const;
	bool hasAttributesOne(const std::vector<QString> & attrs) const;
	QString get(const QString & attribute) const;
	std::vector<std::vector<QString>> getAuthors() const { return authors; }

	Bib(const QString & bibContents);
	void fromFile(const QString & filePath);
	void fromContents(const QString & contents);

private:
	mutable std::map<QString, QString> dt{};
	std::vector<std::vector<QString>> authors{};
	Style format{};
	QString style{};
};








class BibSubset
{
private:
	std::vector<Bib> bibs{};

public:
	BibSubset(const std::vector<Bib> & bibVec);

	BibSubset & sortBy(const QString & attribute);
	BibSubset & sortByYear(bool newFirst = true);
	BibSubset & sortByAuthor();
	BibSubset & removeIf(const QString & attribute, const QString & value);
	BibSubset & removeIf(bool (*func)(bib::Bib));

	void toFile(const QString & outPath);
};






class BibTable
{
private:
	std::vector<Bib> bibs{};

public:
	BibTable(const QString & path, const QStringList & filters = {"*.txt"});

	BibSubset byYear(int yr);
	BibSubset byAuthor(const QString & auth);
	BibSubset byAuthorsAll(const std::vector<QString> & auths);
	BibSubset byAuthorsOne(const std::vector<QString> & auths);
};






void addMedAbbr(const QString & filesDir,
				const QStringList & filters);

void addSquareBrackets(const QString & filesDir,
					   const QStringList & filters);

void correctZhVND(const QString & filesDir,
				  const QStringList & filters);


/// deprecate
void manyFilesToOne(const QString & workPath,
					const QStringList & filters,
					const QString outFilePath);


const std::map<QString, QString> months
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
