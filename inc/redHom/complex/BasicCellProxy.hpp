#ifndef CELL_PROXY_HPP
#define CELL_PROXY_HPP

#include <boost/ref.hpp>

template<typename CellImplT>
class BasicCellProxy {
protected:
  //  boost::reference_wrapper<CellImplT> nonConstImpl;

  mutable CellImplT impl;

public:


  typedef typename CellImplT::Color Color;
  typedef typename CellImplT::Dim Dim;
  typedef typename CellImplT::Id Id;

  typedef CellImplT Impl;

  BasicCellProxy(): impl() {} //, nonConstImpl(impl) {}

  BasicCellProxy(const CellImplT& _impl): impl(_impl) {} //, nonConstImpl(impl) {}

  template<typename ImplT2>
  BasicCellProxy(const ImplT2& _impl): impl(_impl) {}

  Color getColor() const{
	 return impl.getColor();
  }

  template<Color color>
  void setColor() const {
	 impl.template setColor<color>();
  }

  void setColor(const Color& color) const {
	 impl.setColor(color);
  }

  Dim getDim() const {
	 return impl.getDim();
  }

  Id getId() const {
	 return impl.getId();
  }

  bool operator<(const BasicCellProxy& b) const {
	 return impl < b.impl;
  }

  CellImplT* getImpl() const {
	 //return nonConstImpl.get_pointer();
	 return &(const_cast<BasicCellProxy*>(this)->impl);
  }
};

template<typename CellImplT>
class BasicCellProxy<CellImplT*> {
protected:
  mutable CellImplT* impl;
  //  boost::reference_wrapper<CellImplT> nonConstImpl;
public:
  typedef typename CellImplT::Color Color;
  typedef typename CellImplT::Dim Dim;
  typedef typename CellImplT::Id Id;

  typedef CellImplT Impl;

  BasicCellProxy(): impl(NULL) {} //, nonConstImpl(*impl) {}
  BasicCellProxy(CellImplT* _impl): impl(_impl) {} //, nonConstImpl(*impl) {}

  BasicCellProxy(const BasicCellProxy<CellImplT>& other): impl(other.getImpl()) {}
																//nonConstImpl(*impl) {}
  Id getId() const
  {
  	 return impl->getId();
  }

  Color getColor() const{
	 return impl->getColor();
  }

  template<Color color>
  void setColor() const {
	 impl->template setColor<color>();
  }

  void setColor(const Color& color) const {
	 impl->setColor(color);
  }

  Dim getDim() const {
	 return impl->getDim();
  }

  bool operator<(const BasicCellProxy& b) const {
	 return *impl < *b.impl;
  }

  CellImplT* getImpl() const {
	 return const_cast<BasicCellProxy*>(this)->impl;
  }

};

template<typename CellImplT>
class BasicCellProxy<BasicCellProxy<CellImplT> >: public BasicCellProxy<CellImplT> {

  BasicCellProxy();
  BasicCellProxy(const BasicCellProxy&);
};

#endif
