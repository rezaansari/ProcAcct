// This may look like C code, but it is really -*- C++ -*-
//  Pour la gestion des signaux a travers des exceptions

#ifndef SPYPASIGHANDLE_SEEN
#define SPYPASIGHANDLE_SEEN

#include <exception>
using namespace std;

// namespace SOPHYA {
class SigCatch : public std::exception {
public:
  explicit SigCatch(const char* m, int ident=0) throw() ; 
  virtual ~SigCatch() throw() { }
  //! Implementation of std::exception what() method, returning the exception message
  virtual const char* what() const throw();
  //! Returns the associated error-id
  inline int Id() const { return id_; }
private:
  char msg_[128];
  int    id_;
};


void SpyPAConfigureSignalhandling(bool cfpe=false, bool csegv=false, 
                                   bool cint=false, bool cquit=false, 
                                   bool cusr1=false, bool cusr2=false); 
// }  namespace SOPHYA

#endif
