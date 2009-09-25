#ifndef COMPUTEUNIT_H
# define COMPUTEUNIT_H

class ComputeUnit
{
public:
  ComputeUnit () : output_(0), level_(0) {};
  ComputeUnit (double output, int level) : output_(output), level_(level) {};
   
  virtual ~ComputeUnit () {};

  inline
  double output () { return output_; }
  
  inline
  int level_get () { return level_; }

protected:
  double output_;
  int level_;
};

bool operator== (ComputeUnit& n1, ComputeUnit& n2);

#endif
