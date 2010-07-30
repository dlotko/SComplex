#ifndef __COLORED_OBJECTS_LIST_MODEL_HPP_
#define __COLORED_OBJECTS_LIST_MODEL_HPP_

#include <vector>
#include <list>
#include <boost/range.hpp>
#include <boost/ref.hpp>

namespace Util {
  namespace Colors {


	 template<typename Object, typename Color>
	 class ColoredObjectsListModel {

		template<typename T>
		struct Unref: public std::unary_function<const boost::reference_wrapper<T>&, typename boost::unwrap_reference<boost::reference_wrapper<T> >::type&> {
		  typename boost::unwrap_reference<boost::reference_wrapper<T> >::type& operator()(const boost::reference_wrapper<T>& t) const {
			 return unwrap_ref(t);
		  }
		};

	 public:
		typedef std::vector<Object> Objects;	 
		typedef std::list<boost::reference_wrapper<Object> > ObjectPtrs;
		typedef std::vector<ObjectPtrs> ObjectPtrsByColor;
		typedef typename ObjectPtrs::iterator ObjectPtrsIterator;

		typedef typename boost::sub_range<Objects> AllObjects;
		typedef Util::Iterators::RangeTransform<ObjectPtrs, Unref<Object> > ObjectsInColor;

		AllObjects allObjects() {
		  return AllObjects(objects);
		}

		ObjectsInColor objectsInColor(const Color& color) {
		  return ObjectsInColor(objectsByColor[color], Unref<Object>());
		}
		
		void init(size_t colors, size_t size) {
		  objectsByColor.resize(colors);
		  objects.reserve(size);
		}

		ObjectPtrsIterator add(const Object& object, const Color& color) {
		  objects.push_back(object);
		  return objectsByColor[color].insert(objectsByColor[color].end(), boost::ref(objects.back()));	 
		}

		void changeColor(const ObjectPtrsIterator& it, const Color& oldColor, const Color& newColor) {
		  objectsByColor[newColor].splice(objectsByColor[newColor].end(), objectsByColor[oldColor], it);
		}

	 private:
		Objects objects;
		ObjectPtrsByColor objectsByColor;
	 };
  
  }
}

#endif
