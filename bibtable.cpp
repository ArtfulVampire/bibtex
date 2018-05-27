#include "lib.h"

#include <QRegularExpression>

namespace bib
{

BibTable::BibTable(const QString & path, const QStringList & filters)
{
	for(const QString & fn : QDir(path).entryList(filters))
	{
		bibs.push_back(bib::Bib(path + "/" + fn));
	}
}

BibSubset BibTable::byYear(int yr)
{
	std::vector<bib::Bib> res{};
	for(const bib::Bib & inBib : bibs)
	{
		if(inBib.year() == yr) { res.push_back(inBib); }
	}
	return bib::BibSubset(res);
}

BibSubset BibTable::byAuthor(const QString & auth)
{
	std::vector<bib::Bib> res{};
	for(const bib::Bib & inBib : bibs)
	{
		for(const auto & inAuth : inBib.getAuthors())
		{
			if(inAuth[0].contains(auth)) { res.push_back(inBib); }
		}
	}
	return bib::BibSubset(res);
}

BibSubset BibTable::byAuthorsAll(const std::vector<QString> & auths)
{
	std::vector<bib::Bib> res{};
	for(const bib::Bib & inBib : bibs)
	{
		bool ok = true;
		for(const QString & auth : auths)
		{
			bool subOk = false;
			for(const auto & inAuth : inBib.getAuthors())
			{
				if(inAuth[0].contains(auth)) { subOk = true; break; }
			}
			if(!subOk) { ok = false; break; }
		}
		if(!ok) { continue; }

		res.push_back(inBib);
	}
	return bib::BibSubset(res);
}

BibSubset BibTable::byAuthorsOne(const std::vector<QString> & auths)
{
	std::vector<bib::Bib> res{};
	for(const bib::Bib & inBib : bibs)
	{
		bool ok = false;
		for(const QString & auth : auths)
		{
			for(const auto & inAuth : inBib.getAuthors())
			{
				if(inAuth[0].contains(auth)) { ok = true; break; }
			}
			if(ok) { break; }
		}
		if(!ok) { continue; }

		res.push_back(inBib);
	}
	return bib::BibSubset(res);
}




} // end namespace bib
