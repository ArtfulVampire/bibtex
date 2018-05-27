#include "lib.h"
#include <functional>

#include <QRegularExpression>

namespace bib
{

BibSubset::BibSubset(const std::vector<Bib> & bibVec) : bibs(bibVec)
{

}

void BibSubset::toFile(const QString & outPath)
{
	QFile out(outPath);
	out.open(QIODevice::WriteOnly);

	for(Bib & in : this->bibs)
	{
//		std::cout << in.getFilePath() << std::endl;
		out.write((in.asStyle() + "\n\n").toUtf8());
	}
	out.close();
}


BibSubset & BibSubset::sortBy(const QString & attribute)
{
	std::sort(std::begin(bibs),
			  std::end(bibs),
			  [&attribute](const auto & in1, const auto & in2)
	{
		return in1.get(attribute) < in2.get(attribute);
	});
	return *this;
}

BibSubset & BibSubset::sortByYear(bool newFirst)
{
//	using typ = std::binary_function <int, int, bool>;
	using typ = std::function<bool(int,int)>;
	typ comp = (newFirst) ? (static_cast<typ>(std::greater_equal<int>()) ) : (static_cast<typ>(std::less_equal<int>()) );
	std::sort(std::begin(bibs),
			  std::end(bibs),
			  [&comp](const auto & in1, const auto & in2)
	{
		return comp(in1.year(), in2.year());
	});
	return *this;
}

BibSubset & BibSubset::sortByAuthor()
{
	std::sort(std::begin(bibs),
			  std::end(bibs),
			  [](const auto & in1, const auto & in2)
	{
		return in1.firstAuthor() < in2.firstAuthor();
	});
	return *this;
}

BibSubset & BibSubset::removeIf(const QString & attribute, const QString & value)
{
	for(auto it = std::begin(bibs); it != std::end(bibs); ++it)
	{
		if((*it).get(attribute) == value)
		{
			it = bibs.erase(it);
			--it;
		}
	}
	return *this;
}


BibSubset & BibSubset::removeIf(bool (*func)(const bib::Bib &))
{
	for(auto it = std::begin(bibs); it != std::end(bibs); ++it)
	{
		if(func(*it))
		{
			it = bibs.erase(it);
			--it;
		}
	}
	return *this;
}

} // end namespace bib
