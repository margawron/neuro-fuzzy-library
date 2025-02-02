/** @file */ 

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "../common/datum.h"
#include "../common/number.h"
#include "../service/debug.h"
#include "../service/exception.h"


bool ksi::datum::isTypicalityOn() const
{
    return _typicalityON;
}


ksi::datum::datum()
{
   pDecision = nullptr;
}


ksi::datum::~datum()
{
   for (auto & p : attributes)
      delete p;
   if (pDecision)
       delete pDecision;
}

ksi::datum* ksi::datum::clone() const
{
   return new ksi::datum(*this);
}


ksi::datum::datum(const ksi::datum & d)
{
    try 
    {
        for (auto & p : d.attributes)
        {
            if (p)
                    attributes.push_back(p->clone());
            else 
                throw std::string ("p == nullptr, empty desciptor");
        }
        if (d.pDecision)
            pDecision = d.pDecision->clone();
        else 
            pDecision = nullptr;
        
        MISSING_VALUE_SYMBOL = d.MISSING_VALUE_SYMBOL;
        _weight = d._weight;
        _typicality = d._typicality;
        _typicalityON = d._typicalityON;
        _id = d._id;
        _id_incomplete = d._id_incomplete;
    }
    CATCH;
}

ksi::datum::datum(ksi::datum && d) : pDecision (nullptr)
{
   std::swap (attributes, d.attributes);
   std::swap (pDecision, d.pDecision);
   std::swap (MISSING_VALUE_SYMBOL, d.MISSING_VALUE_SYMBOL);
   std::swap (_weight, d._weight);
   
   std::swap (_typicality, d._typicality);
   std::swap (_typicalityON, d._typicalityON);   
   std::swap (_id, d._id);
   std::swap (_id_incomplete, d._id_incomplete);
}


ksi::datum& ksi::datum::operator=(const ksi::datum & d)
{
    try 
    {
        if (this == & d)
            return *this;
        
        for (auto & p : attributes)
            delete p;
        if (pDecision)
            delete pDecision;
        
        attributes.clear();
        
        for (auto & p : d.attributes)
        {
            if (p)
                attributes.push_back(p->clone());  
            else 
                throw std::string ("empty desciptor (null pointer)");
        }
        if (d.pDecision)
            pDecision = d.pDecision->clone();
        else
            pDecision = nullptr;
        
        MISSING_VALUE_SYMBOL = d.MISSING_VALUE_SYMBOL;
        _weight = d._weight;
        _typicality = d._typicality;
        _typicalityON = d._typicalityON;
        _id = d._id;
        _id_incomplete = d._id_incomplete;
        
        return *this;
    }
    CATCH;
}


ksi::datum & ksi::datum::operator=(ksi::datum && d)
{
   if (this == & d)
      return *this;
   
   std::swap (attributes, d.attributes);
   std::swap (pDecision, d.pDecision);
   std::swap (MISSING_VALUE_SYMBOL, d.MISSING_VALUE_SYMBOL);
   std::swap (_weight, d._weight);
   
   std::swap (_typicality, d._typicality);
   std::swap (_typicalityON, d._typicalityON);
   std::swap (_id, d._id);
   std::swap (_id_incomplete, d._id_incomplete);
   
   return *this;
}

ksi::number * ksi::datum::at (std::size_t attr) const 
{
   try 
   {
      if (attr < 0 and attributes.size () <= attr)
      {
         std::stringstream ss;
         ss << "Incorrect index, attr == " << attr << "." << std::endl;
         ss << "Valid values: 0, ... , " << attributes.size() - 1 << std::endl;
         throw ss.str();
      }
      else
         return attributes[attr];
   }
   CATCH;
}

std::size_t ksi::datum::getNumberOfAttributes() const
{
   return attributes.size();
}

void ksi::datum::push_back(const ksi::number& d)
{
   attributes.push_back(d.clone());
}

void ksi::datum::push_back(ksi::number* p)
{
   attributes.push_back(p);
}

std::pair<ksi::datum, ksi::datum> ksi::datum::splitDatum(std::size_t last_index) const
{
   ksi::datum left, right;
   std::size_t nAttr = attributes.size();
   if (last_index >= nAttr)
      last_index = nAttr - 1;
   else if (last_index < 0)
      last_index = 0;
   
   std::size_t a;
   for (a = 0; a < last_index; a++)
      left.push_back(* attributes[a]);
   for (a = last_index; a < nAttr; a++)
      right.push_back(* attributes[a]);
   
   return { left, right };
}

std::pair<ksi::datum, ksi::datum> ksi::datum::cutOffLastAttribute() const
{
   std::size_t nAttr = attributes.size();
   return splitDatum(nAttr - 1); 
}



std::vector< double > ksi::datum::getVector() const
{
   std::vector<double> attrs;
   for (ksi::number * a : attributes)
      attrs.push_back(a->getValue());
   
   return attrs;
}

double ksi::datum::getWeight() const
{
   return _weight;
}

void ksi::datum::setWeight(double weight)
{
   _weight = std::max (0.0, std::min (1.0, weight));
}

double ksi::datum::getTypicality() const
{
   return _typicality;
}

void ksi::datum::setTypicality(double typicality)
{
   _typicality = std::max (0.0, std::min (1.0, typicality));
   _typicalityON = true;
}

void ksi::datum::switchTypicality(bool on)
{
   _typicalityON = on;
}

long int ksi::datum::getID() const
{
   return _id;
}

void ksi::datum::setID(std::size_t id)
{
   _id = id;
}

long int ksi::datum::getIDincomplete() const
{
   return _id_incomplete;
}

void ksi::datum::setIDincomplete(long int id_incomplete)
{
   _id_incomplete = id_incomplete;
}



namespace ksi
{
   std::ostream& operator<<(std::ostream& ss, const ksi::datum & d)
   {
      const int WIDTH = 10;
      auto cols = d.getNumberOfAttributes();
      
      for (std::size_t k = 0; k < cols; k++)
      {
         if (d.at(k)->exists())
            ss << std::setw(WIDTH) << *(d.at(k));
         else
            ss << std::setw(WIDTH) << d.MISSING_VALUE_SYMBOL;
      }
      
      if (d._typicalityON)
      {
         ss << " (typicality == " << d.getTypicality() << ")";
      }
      if (d.pDecision)
          ss <<" (decision == " << *d.pDecision << ")"; 
      
      //ss << " id == " << d.getID() << ", id_incomplete == " << d.getIDincomplete();
      
      
      return ss;
   }
}


void ksi::datum::setDecision(const ksi::number& p)
{
    if (pDecision)
        delete pDecision;
    pDecision = p.clone();
}

void ksi::datum::setDecision(const ksi::number* p)
{
    if (pDecision)
        delete pDecision;
    if (p)
        pDecision = p->clone();
    else
        pDecision = nullptr;
}

const ksi::number * ksi::datum::getDecision() const
{
    return pDecision;
}

