#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <sstream>

using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that)
{
   stringstream out;
   string temp;
   out << that;
   out >> temp;

   for (char digit : temp)
   {
      ubigvalue_t::iterator it = ubig_value.begin();
      ubig_value.insert(it, digit);
      if (!isdigit(digit))
         throw invalid_argument ("ubigint::ubigint(" + temp + ")");
   }
}

ubigint::ubigint (const string& that)  
{
   for (char digit : that)
   {
      ubigvalue_t::iterator it = ubig_value.begin();
      ubig_value.insert(it, digit);
      if (!isdigit(digit))
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
   }
}

ubigint ubigint::operator+(const ubigint& that) const 
{
   ubigint temp;
   int loop;  
   int longer;
   int carry = 0;
   string str = "";
   int remainder;
   int x, y;
   if (this->ubig_value.size() > that.ubig_value.size())
   {
      longer = this->ubig_value.size();
      loop = that.ubig_value.size();
      str = "this";
   }
   else if(this->ubig_value.size() < that.ubig_value.size())
   {
      longer = that.ubig_value.size();
      loop = this->ubig_value.size();
      str = "that";
   }
   else
   {
      loop = this->ubig_value.size();
      longer = that.ubig_value.size();
      str = "";
   }
   for(int i = 0; i < loop; i++)
   {
      x = this->ubig_value[i] - 48; //convert to unsigned_char
      y = that.ubig_value[i] - 48; //convert to unsigned_char
      int sum = x + y + carry;
      remainder = sum % 10;
      unsigned char char_remainder = remainder + '0';
      if (sum >= 10)
      {
         carry = 1;
         temp.ubig_value.push_back(char_remainder);
      }
      else
      {
         carry = 0;
         temp.ubig_value.push_back(char_remainder);
      }
   }
   if (str != "")
   {
      if (str == "this")
      {
         for (int i = loop; i < longer; ++i) 
         {
            x = this->ubig_value[i] - 48;
            int sum = x + carry;
            remainder = sum % 10;
            unsigned char char_remainder = remainder + '0';
            if (sum >= 10)
            {
               carry = 1;
               temp.ubig_value.push_back(char_remainder);
            }
            else
            {
               carry = 0;
               temp.ubig_value.push_back(char_remainder);
            }
         }
      }
      else
      {
         for (int i = loop; i < longer; ++i)
         {
            x = that.ubig_value[i] - 48;
            int sum = x + carry;
            remainder = sum % 10;
            unsigned char char_remainder = remainder + '0';
            if (sum >= 10)
            {
               carry = 1;
               temp.ubig_value.push_back(char_remainder);
            }
            else
            {
               carry = 0;
               temp.ubig_value.push_back(char_remainder);
            }
         }
      }
   }
   if (carry == 1)
   {
      unsigned char carry_ = carry + '0';
      temp.ubig_value.push_back(carry_);
   }
   return temp;
}

ubigint ubigint::operator- (const ubigint& that) const
{
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigvalue_t this_vector = this->ubig_value;
   ubigvalue_t that_vector = that.ubig_value;
   ubigint temp;
   string number = "";
   int x, y;
   int diff;
   unsigned char char_difference;
   if (this->ubig_value.size() > that.ubig_value.size())
   {
      unsigned char c = 0 + '0';
      for (unsigned int i = that.ubig_value.size(); i <= this->ubig_value.size(); i++)
         that_vector.push_back(c);
   }
   else if(this->ubig_value.size() < that.ubig_value.size())
   {
      unsigned char c = 0 + '0';
      for(unsigned int i = this->ubig_value.size(); i <= that.ubig_value.size(); i++)
         this_vector.push_back(c);
   }
   for(unsigned int i = 0; i < this_vector.size(); i++)
   {
      x = this_vector[i] - 48; //convert to unsigned_char
      y = that_vector[i] - 48; //convert to unsigned_char
      if (x < y)
      {
         if (this_vector[i + 1] == 0 && this_vector[i] == 0)
            this_vector[i + 1] = 9;
         x += 10;
         this_vector[i + 1] -= 1;
         diff = x - y;
         char_difference = diff + '0';
      }
      else
      {
         diff = x - y;
         char_difference = diff + '0'; 
      }
      temp.ubig_value.push_back(char_difference);
   }
   for(int i = temp.ubig_value.size() - 1; i >= 0; --i)
   {
      if(temp.ubig_value.size() == 1 || temp.ubig_value[i-1] == '\0')
         break;
      if(temp.ubig_value[i] == '0' && temp.ubig_value[i-1] == '0')
         temp.ubig_value.pop_back();
      else if(temp.ubig_value[i] == '0' && temp.ubig_value[i-1] != '0')
      {
         temp.ubig_value.pop_back();
         break;
      }
      else
         break;
   }
   return temp;
}

ubigint ubigint::operator* (const ubigint& that) const 
{
   ubigint temp;
   vector<unsigned char> output(this->ubig_value.size() + that.ubig_value.size(), '0');
   temp.ubig_value = output;
   int c, d;
   for(unsigned int i = 0; i < this->ubig_value.size(); i++)
   {
      c = 0;
      for(unsigned int index = 0; index < that.ubig_value.size(); index++)
      {
         d = (temp.ubig_value[i + index] - 48) + ((this->ubig_value[i] - 48) * (that.ubig_value[index] - 48)) + c;
         temp.ubig_value[i + index] = ((d % 10) + '0');
         c = floor(d / 10);
      }
      temp.ubig_value[i + that.ubig_value.size()] = c + '0';
   }
   for(int i = temp.ubig_value.size() - 1; i >= 0; --i)
   {
      if(temp.ubig_value.size() == 1 || temp.ubig_value[i-1] == '\0')
         break;
      if(temp.ubig_value[i] == '0' && temp.ubig_value[i-1] == '0')
         temp.ubig_value.pop_back();
      else if(temp.ubig_value[i] == '0' && temp.ubig_value[i-1] != '0')
      {
         temp.ubig_value.pop_back();
         break;
      }
      else
         break;
   }
   return temp;
}

void ubigint::multiply_by_2()
{
   vector<unsigned char> output(this->ubig_value.size() + 1, '0');
   vector<unsigned char> that;
   that.push_back('2');
   ubigint temp;
   temp.ubig_value = output;
   int x, y;
   for(unsigned int i = 0; i < this->ubig_value.size(); i++)
   {
      x = 0;
      for(unsigned int index = 0; index < that.size(); index++)
      {
         y = (temp.ubig_value[i + index] - 48) + ((this->ubig_value[i] - 48) * (that[index] - 48)) + x;
         temp.ubig_value[i + index] = ((y % 10) + '0');
         x = floor(y / 10);
      }
      temp.ubig_value[i + that.size()] = x + '0';
   }
   for(int i = temp.ubig_value.size() - 1; i >= 0; --i){
      if(temp.ubig_value.size() == 1 || temp.ubig_value[i-1] == '\0')
         break;
      if(temp.ubig_value[i] == '0' && temp.ubig_value[i-1] == '0')
         temp.ubig_value.pop_back();
      else if(temp.ubig_value[i] == '0' && temp.ubig_value[i-1] != '0')
      {
         temp.ubig_value.pop_back();
         break;
      }
      else
         break;
   }
   this->ubig_value.clear();
   this->ubig_value = temp.ubig_value;
}

struct quo_rem { ubigint quotient; ubigint remainder; };

quo_rem ubigint::udivide (const ubigint dividend,
          ubigint divisor) const 
{
   ubigint zero;
   ubigint power_of_2;
   power_of_2.ubig_value.push_back('1');
   ubigint quotient;
   quotient.ubig_value.push_back('0');
   ubigint remainder;
   remainder.ubig_value = dividend.ubig_value;
   zero.ubig_value.push_back('0');
   if (divisor == zero)
   {
      divisor.ubig_value.clear();
      power_of_2.ubig_value.clear();
      quotient.ubig_value = zero.ubig_value;
      remainder.ubig_value = zero.ubig_value;
      quo_rem ans {.quotient = quotient, .remainder = remainder};
      return ans;
   }
   while(divisor < dividend)
   {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while(zero < power_of_2)
   {
      if (divisor < remainder || (divisor == remainder))
      {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   divisor.ubig_value.clear();
   power_of_2.ubig_value.clear();
   quo_rem ans {.quotient = quotient, .remainder = remainder};
   quotient.ubig_value.clear();
   remainder.ubig_value.clear();
   return ans;
}

void ubigint::divide_by_2() {
   ubigint two;
   two.ubig_value.push_back('2');
   vector<unsigned char> result;
   int c, s;
   int d = this->ubig_value.back() - 48;

   for(int i = this->ubig_value.size() - 1; i >= 0; --i)
   {
      if(d >= 2)
      {
         c = d / 2;
         result.push_back(c + '0');
         if(i == 0)
             break;
         else
         {
            s = d - (2 * c);
            d = (s * 10) + (this->ubig_value[i - 1] - 48);
         }
      }
      else
      {
         if(d == 0)
         {
            result.push_back('0');
            d = (this->ubig_value[i - 1] - 48);
         }
         else
         {
            result.push_back('0');
            d = (this->ubig_value[i - 1] - 48) + (d * 10);
         }
      }
   }
   this->ubig_value.clear();
   for(int it = result.size() - 1; it >= 0; --it)
      this->ubig_value.push_back(result[it]);
   for(int index = this->ubig_value.size() - 1;
    index >= 0; --index)
   {
      if (this->ubig_value[index] != '0')
         break;
      else
         this->ubig_value.pop_back();
   }
}

ubigint ubigint::operator/ (const ubigint& that) const
{
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const
{
   return udivide(*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const
{
   int size1 = this->ubig_value.size();
   int size2 = that.ubig_value.size();

   bool equal = false;
   bool same_size = size1 == size2;

   if(same_size)
   {
      equal = true;
      for(int i = 0; i < size1; ++i)
      {
         if(this->ubig_value[i] != that.ubig_value[i])
         {
            equal = false;
            break;
         }
      }
   }
   return equal;
}

bool ubigint::operator< (const ubigint& that) const 
{
   unsigned int size = this->ubig_value.size();
   if(size < that.ubig_value.size())
      return true;
   else if(size > that.ubig_value.size())
      return false;
   else
   {
      for(int i = size - 1; i >= 0; --i)
      {
         if((this->ubig_value[i] != that.ubig_value[i]) && (this->ubig_value[i] > that.ubig_value[i]))
            return false;
         if((this->ubig_value[i] != that.ubig_value[i]) && (this->ubig_value[i] < that.ubig_value[i]))
            return true;
      }
   }
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) 
{ 
   unsigned int count = 69; 
   unsigned int size;
   vector<char> output;
   string str;

   if (that.ubig_value.empty())
      return out << "0";
   for (int i : that.ubig_value)
      output.insert(output.begin(), i);
   for (char j : output)
      str += j;
   size = output.size();
   while (count < size)
   {
      str.insert(count, "\\");
      str.insert(count+1, "\n");
      size += 2;
      count += 71;
   }
   return out << str;  
} 
