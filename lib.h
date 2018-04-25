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
	{"<A>", "author"},
	{"<T>", "title"},
	{"<Y>", "year"},
	{"<J>", "journal"},
	{"<j>", "abbr"},
	{"<V>", "volume"},
	{"<N>", "number"},
	{"<P>", "pages"},
	{"<D>", "doi"},
	{"<M>", "PMID"},
	{"<S>", "publisher"},
	{"<C>", "city"},
	{"<B>", "booktitle"},
	{"<O>", "organization"}
};
const QString articleStyle = "<A>. <T>. <j>. <Y>[;<V>][(<N>)][:<P>].[ https://doi.org/<D>.][ Cited in PubMed PMID:<M>.]";


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