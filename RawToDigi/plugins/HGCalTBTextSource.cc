#include <iostream>
#include "HGCal/RawToDigi/plugins/HGCalTBTextSource.h"
using namespace std;

bool HGCalTBTextSource::readLines()
{
	m_lines.clear();
	char buffer[1024];
	while (!feof(m_file)) {
		buffer[0] = 0;
		fgets(buffer, 1000, m_file);
		if (strstr(buffer, "DONE")) break; // done with this event!
		if (buffer[0] != '0' && buffer[1] != 'x') continue;
		m_lines.push_back(buffer);
	}
	return !m_lines.empty();
}

void HGCalTBTextSource::produce(edm::Event & e)
{
	std::auto_ptr<FEDRawDataCollection> bare_product(new  FEDRawDataCollection());


	// here we parse the data
	std::vector<uint16_t> skiwords;
	// make sure there are an even number of 32-bit-words (a round number of 64 bit words...
	if (m_lines.size() % 2) {
		skiwords.push_back(0);
		skiwords.push_back(0);
	}
	for (std::vector<std::string>::const_iterator i = m_lines.begin(); i != m_lines.end(); i++) {
		uint32_t a, b;
		sscanf(i->c_str(), "%x %x", &a, &b);
                cout<<endl<<"a = "<<a<<" b = "<<b<<endl;
		skiwords.push_back(uint16_t(b >> 16));
		skiwords.push_back(uint16_t(b));
	}

	FEDRawData& fed = bare_product->FEDData(m_sourceId);
	size_t len = sizeof(uint16_t) * skiwords.size();
	fed.resize(len);
	memcpy(fed.data(), &(skiwords[0]), len);

	e.put(bare_product);
}



void HGCalTBTextSource::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
	edm::ParameterSetDescription desc;
	desc.setComment("TEST");
	desc.addUntracked<int>("run", 101);
	desc.addUntracked<std::vector<std::string> >("fileNames");
	descriptions.add("source", desc);
}


#include "FWCore/Framework/interface/InputSourceMacros.h"

DEFINE_FWK_INPUT_SOURCE(HGCalTBTextSource);
