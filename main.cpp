#include <QCoreApplication>
#include "lib.h"
#include <QRegularExpression>

using namespace bib;

int main()
{
	if(0)
	{
		const QString workPath = "/media/Files/Dropbox/IHNA/PapersWorksBooks/OurLabPapers/PapersToPublish/GalyaTBIspaces/bibtex";
		const QStringList filters = {"*.txt"};
		const QString outFilePath = workPath + "/base.bib";
		bib::manyFilesToOne(workPath, filters, outFilePath);
	}

	const QString prr2 =
R"(@article{ogata1995,
title={Human EEG responses to classical music and simulated white noise: effects of a musical loudness component on consciousness},
author={Ogata, Shigeki},
journal={Perceptual and motor skills},
volume={80},
number={3},
pages={779--790},
year={1995},
publisher={SAGE Publications Sage CA: Los Angeles, CA},
doi={10.2466/pms.1995.80.3.779},
PMID={7567395}
},

@article{eghwrudjakpor2010,
title={Decompressive craniectomy following brain injury: factors important to patient outcome},
author={Eghwrudjakpor, PatrickO and Allison, AkaribariB},
journal={Libyan Journal of Medicine},
volume={5},
number={1},
pages={4620},
year={2010},
publisher={Taylor \& Francis},
doi={10.3402/ljm.v5i0.4620},
PMID={28156293}
},

@article{frick1985,
title={Communicating emotion: The role of prosodic features.},
author={Frick, Robert W},
journal={Psychological Bulletin},
volume={97},
number={3},
pages={412--429},
year={1985},
publisher={American Psychological Association},
doi={10.1037/0033-2909.97.3.412}
},

@book{ledoux1998,
title={The emotional brain: The mysterious underpinnings of emotional life},
author={LeDoux, Joseph},
year={1998},
publisher={Simon and Schuster},
city={New York},
ISBN={978-0-684-80382-1}
},


@article{lezak1986,
title={Psychological implications of traumatic brain damage for the patient's family.},
author={Lezak, Muriel D},
journal={Rehabilitation Psychology},
volume={31},
number={4},
pages={241--250},
year={1986},
publisher={US: Springer Publishing},
doi={10.1037/h0091551}
}


)";


	bib::BibBase bas(prr2);

	for(const auto & in : bas.asStyle())
	{
		std::cout << in << std::endl << std::endl;
	}


	const QString prr =
R"(
@article{frick1985,
title={Communicating emotion: The role of prosodic features.},
author={Frick, Robert W},
journal={Psychological Bulletin},
volume={97},
number={3},
pages={412--429},
year={1985},
publisher={American Psychological Association},
doi={10.1037/0033-2909.97.3.412}
},
)";

//	bib::Bib beeb(prr);
//	std::cout << beeb.asStyle() << std::endl;




	return 0;
}
