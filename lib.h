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

const QString authorStyle = "<L> <Fs><Ss>"; /// L-last name, F - first name, S - second name, s - short
const QString authorSeparator = ", ";
const int authorHowManyEtAl = 11;


inline std::ostream & operator<< (std::ostream & os, const QString & toOut)
{
	os << toOut.toStdString();
	return os;
}


const std::vector<std::pair<QString, QString>> styleAcronyms
{
	{"<Auth>", "author"},
	{"<Tit>", "title"},
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
	{"<Cit>", "city"},
	{"<Co>", "country"},
	{"<St>", "state"},
	{"<DOI>", "doi"},
	{"<PMID>", "PMID"},
	{"<ISBN>", "ISBN"}
};

#if 01
/// Taylor and Francis - https://www.tandfonline.com/action/authorSubmission?journalCode=ibij20&page=instructions
const QString articleStyle = "<Auth>. <Tit>. <Js>. <y>[;<Vol>][(<Num>)][:<Pag>].[ https://doi.org/<DOI>.][ Cited in PubMed PMID:<PMID>.]";
const QString bookStyle = "<Auth>. <Tit>. <Cit>[ (<St>)]: <Pub>; <y>.[ <Pag> p.][ ISBN <ISBN>]";
const QString confStyle = "<Auth>. <Tit>. Paper presented at: <Conf>; <y> <m> <d>; <Cit>, <Co>.";
#elif 0

#else

#endif


class Bib
{
public:
	QString asStyle(const QString & style = bib::articleStyle);
	int year();
	QString firstAuthor();

	Bib(const QString & bibContents);
private:
	std::map<QString, QString> dt{};
};

class BibBase
{
private:
	std::vector<Bib> bibs;

public:
	BibBase(const QString & baseContents);
	std::vector<QString> asStyle(const QString & style = bib::articleStyle);
};




void manyFilesToOne(const QString & workPath,
					const QStringList & filters,
					const QString outFilePath);

void addMedAbbr(const QString & filesDir,
				const QStringList & filters,
				const QByteArray & medlineBase);



QString authorsFromData(const QString & authors, const QString & style = authorStyle);

} // end namespace bib

#endif // LIB_H