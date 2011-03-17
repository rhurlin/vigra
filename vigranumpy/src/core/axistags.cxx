/************************************************************************/
/*                                                                      */
/*               Copyright 2010-2011 by Ullrich Koethe                  */
/*                                                                      */
/*    This file is part of the VIGRA computer vision library.           */
/*    The VIGRA Website is                                              */
/*        http://hci.iwr.uni-heidelberg.de/vigra/                       */
/*    Please direct questions, bug reports, and contributions to        */
/*        ullrich.koethe@iwr.uni-heidelberg.de    or                    */
/*        vigra@informatik.uni-hamburg.de                               */
/*                                                                      */
/*    Permission is hereby granted, free of charge, to any person       */
/*    obtaining a copy of this software and associated documentation    */
/*    files (the "Software"), to deal in the Software without           */
/*    restriction, including without limitation the rights to use,      */
/*    copy, modify, merge, publish, distribute, sublicense, and/or      */
/*    sell copies of the Software, and to permit persons to whom the    */
/*    Software is furnished to do so, subject to the following          */
/*    conditions:                                                       */
/*                                                                      */
/*    The above copyright notice and this permission notice shall be    */
/*    included in all copies or substantial portions of the             */
/*    Software.                                                         */
/*                                                                      */
/*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND    */
/*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES   */
/*    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND          */
/*    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT       */
/*    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,      */
/*    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      */
/*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR     */
/*    OTHER DEALINGS IN THE SOFTWARE.                                   */
/*                                                                      */
/************************************************************************/

#define PY_ARRAY_UNIQUE_SYMBOL vigranumpycore_PyArray_API
#define NO_IMPORT_ARRAY
#include <vigra/axistags.hxx>
#include <vigra/numpy_array.hxx>
#include <boost/python.hpp>

namespace python = boost::python;

namespace vigra {

template<class T>
inline PyObject * managingPyObject(T *p)
{
    return typename python::manage_new_object::apply<T *>::type()(p);
}

template<class Copyable>
python::object
generic__copy__(python::object copyable)
{
    Copyable* newCopyable(new Copyable(python::extract<const Copyable &>(copyable)()));
    python::object result(python::detail::new_reference(managingPyObject(newCopyable)));

    python::extract<python::dict>(result.attr("__dict__"))().update(copyable.attr("__dict__"));

    return result;
}

template<class Copyable>
python::object
generic__deepcopy__(python::object copyable, python::dict memo)
{
    python::object copyMod = python::import("copy");
    python::object deepcopy = copyMod.attr("deepcopy");
    python::object builtin = python::import("__builtin__");
    python::object globals = builtin.attr("__dict__");
    
    Copyable* newCopyable(new Copyable(python::extract<const Copyable &>(copyable)()));
    python::object result(python::detail::new_reference(managingPyObject(newCopyable)));

    python::dict locals;
    locals["copyable"] = copyable;
    int copyableId = python::extract<int>(python::eval("id(copyable)", globals, locals))();
    memo[copyableId] = result;

    python::object dict_copy = deepcopy(python::extract<python::dict>(copyable.attr("__dict__"))(),
                                        memo);    
    python::extract<python::dict>(result.attr("__dict__"))().update(dict_copy);
    return result;
}

AxisInfo AxisInfo__call__(AxisInfo const & i, double resolution, std::string const & description)
{
    return AxisInfo(i.key(), i.typeFlags(), resolution, description);
}

AxisInfo AxisInfo_x()
{
	return AxisInfo::x();
}

AxisInfo AxisInfo_y()
{
    return AxisInfo::y();
}

AxisInfo AxisInfo_z()
{
    return AxisInfo::z();
}

AxisInfo AxisInfo_t()
{
    return AxisInfo::t();
}

AxisInfo AxisInfo_fx()
{
    return AxisInfo::fx();
}

AxisInfo AxisInfo_fy()
{
    return AxisInfo::fy();
}

AxisInfo AxisInfo_fz()
{
    return AxisInfo::fz();
}

AxisInfo AxisInfo_ft()
{
    return AxisInfo::ft();
}

AxisInfo AxisInfo_c()
{
    return AxisInfo::c();
}

// #if 0
// PyAxisTags
// PyAxisTags_transform(PyAxisTags const & oldTags, python::object index, int lnew)
// {
    // PyAxisTags newTags;
	// python::object ellipsis = python::object(python::detail::borrowed_reference(Py_Ellipsis));
	// int lold = oldTags.size();
    // if(!PySequence_Check(index.ptr()))
    // {
        // index = make_tuple(index);
    // }
    // int lindex = len(index);
    // int lnone = 0, lellipsis = 0;
    // for(int k=0; k<lindex; ++k)
    // {
        // python::object item(index[k]);
        // if(item == python::object())
            // ++lnone;
        // else if(item == ellipsis)
            // ++lellipsis;
    // }
    // lindex -= lnone;
    // if(lindex < lold && lellipsis == 0)
    // {
        // index += make_tuple(ellipsis);
        // ++lindex;
    // }
    // lellipsis = lold - lindex;
    // int knew = 0, kold = 0, kindex = 0;
    // while(knew < lnew)
    // {
        // python::object item = index[kindex];
        // if(PyInt_Check(item.ptr()))
        // {
            // ++kold;
            // ++kindex;
        // }
        // else 
        // {
            // if(item == python::object())
            // {
                // newTags.push_back(python::object(AxisInfo()));
            // }
            // else
            // {
                // newTags.push_back(oldTags[kold]);
                // ++kold;
            // }
            // ++knew;
            // if(lellipsis > 0 && item == ellipsis)
                // --lellipsis;
            // else
                // ++kindex;
        // }
    // }
    // return newTags;
// }
// #endif

AxisTags *
AxisTags_create(python::object i1, python::object i2,
                python::object i3, python::object i4, python::object i5)
{
    std::auto_ptr<AxisTags> res(new AxisTags());
    
    python::extract<AxisTags const &> tags(i1);
    if(tags.check())
    {
        res = std::auto_ptr<AxisTags>(new AxisTags(tags()));
    }
    else if(PySequence_Check(i1.ptr()))
    {
        int size = len(i1);
        for(int k=0; k<size; ++k)
        {
            python::extract<AxisInfo const &> info(i1[k]);
            if(!info.check())
            {
                PyErr_SetString(PyExc_TypeError, "AxisTags(): Argument must be a sequence of AxisInfo objects.");
                python::throw_error_already_set();
            }
            res->push_back(info());
        }
    }
    else if(PyInt_Check(i1.ptr()))
    {
        int size = python::extract<int>(i1)();
        for(int k=0; k<size; ++k)
            res->push_back(AxisInfo());
    }
    else
    {
        if(i1 != python::object())
        {
            python::extract<AxisInfo const &> info(i1);
            if(!info.check())
            {
                PyErr_SetString(PyExc_TypeError, "AxisTags(): Argument must be a sequence of AxisInfo objects.");
                python::throw_error_already_set();
            }
            res->push_back(info());
        }
        if(i2 != python::object())
        {
            python::extract<AxisInfo const &> info(i2);
            if(!info.check())
            {
                PyErr_SetString(PyExc_TypeError, "AxisTags(): Argument must be a sequence of AxisInfo objects.");
                python::throw_error_already_set();
            }
            res->push_back(info());
        }
        if(i3 != python::object())
        {
            python::extract<AxisInfo const &> info(i3);
            if(!info.check())
            {
                PyErr_SetString(PyExc_TypeError, "AxisTags(): Argument must be a sequence of AxisInfo objects.");
                python::throw_error_already_set();
            }
            res->push_back(info());
        }
        if(i4 != python::object())
        {
            python::extract<AxisInfo const &> info(i4);
            if(!info.check())
            {
                PyErr_SetString(PyExc_TypeError, "AxisTags(): Argument must be a sequence of AxisInfo objects.");
                python::throw_error_already_set();
            }
            res->push_back(info());
        }
        if(i5 != python::object())
        {
            python::extract<AxisInfo const &> info(i5);
            if(!info.check())
            {
                PyErr_SetString(PyExc_TypeError, "AxisTags(): Argument must be a sequence of AxisInfo objects.");
                python::throw_error_already_set();
            }
            res->push_back(info());
        }
    }
    
    return res.release();
}

void AxisTags_setitem(AxisTags & tags, int k, AxisInfo const & i)
{
    tags[k] = i;
}

void AxisTags_setitem_key(AxisTags & tags, std::string const & key, AxisInfo const & i)
{
    AxisTags_setitem(tags, tags.index(key), i);
}

// #if 0
// void printAxistags(NumpyAnyArray a)
// {
    // python::object array(python::detail::borrowed_reference(a.pyObject()));
    // python::object tags(getattr(array, "axistags", PyAxisTags()));
	// std::cerr << "Axistags via boost::python:\n";
    // std::cerr << python::extract<PyAxisTags const &>(tags)().repr();

	// std::cerr << "Axistags via C-API:\n";
	// if(PyObject_HasAttrString(a.pyObject(), "axistags"))
	// {
		// python::object tags(python::detail::new_reference(PyObject_GetAttrString(a.pyObject(), "axistags")));
		// std::cerr << python::extract<PyAxisTags const &>(tags)().repr();
	// }
	// else
	// {
		// std::cerr << "attribute 'axistags' missing\n";
	// }
// }
// #endif

void defineAxisTags()
{
    using namespace boost::python;

    enum_<AxisType>("AxisType")
        .value("UnknownAxisType", UnknownAxisType)
        .value("Space", Space)
        .value("Time", Time)
        .value("Channels", Channels)
        .value("Frequency", Frequency)
        .value("Angle", Angle)
    ;

    class_<AxisInfo>("AxisInfo", no_init)
        .def(init<std::string, AxisType, double, std::string>(
             (arg("name")="?", arg("typeFlags")=UnknownAxisType, 
              arg("resolution")=0.0, arg("description")="")))
        .def(init<AxisInfo const &>())
		.def_readonly("key", &AxisInfo::key_)
		.def_readwrite("description", &AxisInfo::description_)
		.def_readwrite("resolution", &AxisInfo::resolution_)
		.def_readonly("typeFlags", &AxisInfo::flags_)
		.def("toFrequencyDomain", &AxisInfo::toFrequencyDomain, (arg("size") = 0))
		.def("fromFrequencyDomain", &AxisInfo::fromFrequencyDomain, (arg("size") = 0))
		.def("isSpatial", &AxisInfo::isSpatial)
		.def("isTemporal", &AxisInfo::isTemporal)
		.def("isChannel", &AxisInfo::isChannel)
		.def("isFrequency", &AxisInfo::isFrequency)
		.def("isAngular", &AxisInfo::isAngular)
		.def("isType", &AxisInfo::isType)
		.def(self == self)
		.def(self != self)
		.def("__copy__", &generic__copy__<AxisInfo>)
		.def("__deepcopy__", &generic__deepcopy__<AxisInfo>)
		.def("__repr__", &AxisInfo::repr)
		.def("__call__", &AxisInfo__call__, (arg("resolution") = 0.0, arg("description") = ""))
        .add_static_property("x", &AxisInfo_x)
        .add_static_property("y", &AxisInfo_y)
        .add_static_property("z", &AxisInfo_z)
        .add_static_property("t", &AxisInfo_t)
        .add_static_property("fx", &AxisInfo_fx)
        .add_static_property("fy", &AxisInfo_fy)
        .add_static_property("fz", &AxisInfo_fz)
        .add_static_property("ft", &AxisInfo_ft)
        .add_static_property("c", &AxisInfo_c)
    ;

	class_<AxisTags >("AxisTags", no_init)
		.def("__init__", make_constructor(&AxisTags_create,
            default_call_policies(),
            (arg("i1")=object(), arg("i2")=object(), arg("i3")=object(), 
             arg("i4")=object(), arg("i5")=object())))
		.def("__repr__", &AxisTags::repr)
		.def("__copy__", &generic__copy__<AxisTags>)
		.def("__deepcopy__", &generic__deepcopy__<AxisTags>)
		.def("__getitem__", (AxisInfo const & (AxisTags::*)(int) const)&AxisTags::operator[],
                             return_value_policy<copy_const_reference>())
		.def("__getitem__", (AxisInfo const & (AxisTags::*)(std::string const &) const)&AxisTags::operator[],
                             return_value_policy<copy_const_reference>())
		.def("__setitem__", &AxisTags_setitem)
		.def("__setitem__", &AxisTags_setitem_key)
    ;
    
// #if 0
// class AxisTags(object):
    
    // def __deepcopy__(self, memo):
        // result = AxisTags()
        // result.tags = copy.deepcopy(self.tags, memo)
        // memo[id(self)] = result
        // return result
    
    // def __repr__(self):
        // return ' '.join(map(lambda x: x.key, self.tags))
        
    // def __eq__(self, other):
        // return self.tags == other.tags
    
    // def __ne__(self, other):
        // return self.tags != other.tags
    
    // def __len__(self):
        // return len(self.tags)
    
    // def __getitem__(self, index):
        // if type(index) is str:
            // index = self.index(index)
        // return self.tags[index]
    
    // def __setitem__(self, index, value):
        // if type(index) is str:
            // index = self.index(index)
        // existing_index = self.index(value.key)
        // if existing_index < len(self) and existing_index != index:
            // raise RuntimeError("AxisTags.__setitem__(): axis key already exists.")
        // self.tags[index] = value
    
    // def __delitem__(self, index):
        // if type(index) is str:
            // index = self.index(index)
        // del self.tags[index]
    
    // def insert(self, index, value):
        // if type(index) is str:
            // index = self.index(index)
        // if self.index(value.key) < len(self):
            // raise RuntimeError("AxisTags.insert(): axis key already exists.")
        // self.tags.insert(index, value)
    
    // def append(self, value):
        // if self.index(value.key) < len(self):
            // raise RuntimeError("AxisTags.append(): axis key already exists.")
        // self.tags.append(value)
    
    // def index(self, key):
        // for k in xrange(len(self.tags)):
            // if self.tags[k].key == key:
                // return k
        // return len(self.tags)
   
    // @property
    // def channelIndex(self):
        // return self.index('c')
   
    // @property
    // def majorNonchannelIndex(self):
        // # FIXME: this must be generalized to the case when 'x' is not present.
        // return self.index('x')
    
    // def axisTypeCount(self, axistype):
        // count = 0
        // for k in self.tags:
            // if k.isType(axistype):
                // count += 1
        // return count
    
    // def permutationToNormalOrder(self):
        // return canonicalAxisPermutation(self.tags)
    
    // def permutationFromNormalOrder(self):
        // return [int(k) for k in numpy.array(map(lambda x: ord(x.key[-1]), self.tags)).argsort().argsort()]
    
    // def setChannelDescription(self, description):
        // index = self.index('c')
        // if index < len(self):
            // self.tags[index].description = description
    
    // def dropChannelDimension(self):
        // index = self.index('c')
        // if index < len(self):
            // del self.tags[index]
    
    // def insertChannelDimension(self):
        // index = self.index('c')
        // if index < len(self):
            // raise RuntimeError("AxisTags.insertChannelDimension(): already have a channel dimension.")
        // if defaultOrder == 'F':
            // self.tags.insert(0, AxisInfo.c)
        // else:
            // self.tags.append(AxisInfo.c)
    
    // def swapaxes(self, i1, i2):
        // self.tags[i1], self.tags[i2] = self.tags[i2], self.tags[i1]
    
    // def transpose(self, permutation = None):
        // l = len(self)
        // if permutation is None:
                // permutation = range(l-1, -1, -1)
        // result = AxisTags([None]*l)
        // for k in xrange(l):
            // result.tags[k] = self.tags[permutation[k]]
        // return result
    
    // def transform(self, index, new_ndim):
        // new_axistags = [AxisInfo() for k in xrange(new_ndim)]
        // old_axistags = self.tags
        // old_ndim = len(old_axistags)

        // try:
            // # make sure that 'index' is a tuple
            // len_index = len(index)
        // except:
            // index = (index,)
            // len_index = 1
        // len_index -= index.count(numpy.newaxis)
        // if len_index < old_ndim and index.count(Ellipsis) == 0:
            // index += (Ellipsis,)
            // len_index += 1
        
        // # how many missing axes are represented by an Ellipsis ?
        // len_ellipsis = old_ndim - len_index
        
        // knew, kold, kindex = 0, 0, 0
        // while knew < new_ndim:
            // try:
                // # if index[kindex] is int, the dimension is bound => drop this axis
                // int(index[kindex]) 
                // kold += 1
                // kindex += 1
            // except:
                // if index[kindex] is not numpy.newaxis:
                    // # copy the tag
                    // new_axistags[knew] = copy.copy(old_axistags[kold])
                    
                    // # adjust the resolution for a possible step in the slice
                    // try:
                        // new_axistags[knew].resolution *= index[kindex].step
                    // except:
                        // pass
                        
                    // kold += 1
                // knew += 1
                // # the first ellipsis represents all missing axes
                // if len_ellipsis > 0 and index[kindex] is Ellipsis:
                    // len_ellipsis -= 1
                // else:
                    // kindex += 1
        
        // return AxisTags(new_axistags)
// #endif

	// // class_<AxisTags<> >("AxisTags", no_init)
		// // .def("__init__", make_constructor(&AxisTags_create,
            // // default_call_policies(),
            // // (arg("i1")=object(), arg("i2")=object(), arg("i3")=object(), 
                   // // arg("i4")=object(), arg("i5")=object())))
		// // .def("__repr__", &AxisTags<>::repr)
		// // .def("__getitem__", (AxisInfo const & (AxisTags<>::*)(int) const)&AxisTags<>::operator[],
                             // // return_value_policy<copy_const_reference>())
		// // .def("__getitem__", (AxisInfo const & (AxisTags<>::*)(std::string const &) const)&AxisTags<>::get,
                             // // return_value_policy<copy_const_reference>())
		// // .def("__setitem__", &AxisTags_setitem)
		// // .def("__setitem__", &AxisTags_setitem_key)
		// // .def("insert", (void (PyAxisTags::*)(int, object))&PyAxisTags::insert)
		// // .def("insert", (void (PyAxisTags::*)(std::string const &, object))&PyAxisTags::insert)
		// // .def("index", (int (PyAxisTags::*)(std::string const &) const)&PyAxisTags::findKey)
		// // .def("append", &PyAxisTags::append)
		// // .def("__delitem__", (void (PyAxisTags::*)(int))&PyAxisTags::dropAxis)
		// // .def("__delitem__", (void (PyAxisTags::*)(std::string const &))&PyAxisTags::dropAxis)
		// // .def("__len__", &PyAxisTags::size)
		// // .def(self == self)
		// // .def(self != self)
		// // .def("axisTypeCount", &PyAxisTags::axisTypeCount)
		// // .def("axesByFlag", &PyAxisTags::axesByFlag)
		// // .def("spatialAxes", &PyAxisTags::spatialAxes)
		// // .def("temporalAxes", &PyAxisTags::temporalAxes)
		// // .def("channelAxes", &PyAxisTags::channelAxes)
		// // .def("frequencyAxes", &PyAxisTags::frequencyAxes)
		// // .def("angularAxes", &PyAxisTags::angularAxes)
		// // .def("untaggedAxes", &PyAxisTags::untaggedAxes)
		// // .def("canonicalOrdering", &PyAxisTags::canonicalOrdering)
		// // .def("matchOrdering", &PyAxisTags::matchOrdering)
		// // .def("transpose", (void (PyAxisTags::*)(object const &))&PyAxisTags::transpose)
		// // .def("transpose", (void (PyAxisTags::*)())&PyAxisTags::transpose)
		// // .def("transform", &PyAxisTags_transform)
    // ;

	// // class_<PyAxisTags>("PyAxisTags", no_init)
        // // .def(init<object const &, object const &, object const &, 
                  // // object const &, object const &>(
                  // // (arg("i1")=object(), arg("i2")=object(), arg("i3")=object(), 
                   // // arg("i4")=object(), arg("i5")=object())))
		// // .def("__repr__", &PyAxisTags::repr)
		// // .def("__getitem__", (object (PyAxisTags::*)(int))&PyAxisTags::getitem)
		// // .def("__getitem__", (object (PyAxisTags::*)(std::string const &))&PyAxisTags::getitem)
		// // .def("__setitem__", (void (PyAxisTags::*)(int, object))&PyAxisTags::setitem)
		// // .def("__setitem__", (void (PyAxisTags::*)(std::string const &, object))&PyAxisTags::setitem)
		// // .def("insert", (void (PyAxisTags::*)(int, object))&PyAxisTags::insert)
		// // .def("insert", (void (PyAxisTags::*)(std::string const &, object))&PyAxisTags::insert)
		// // .def("index", (int (PyAxisTags::*)(std::string const &) const)&PyAxisTags::findKey)
		// // .def("append", &PyAxisTags::append)
		// // .def("__delitem__", (void (PyAxisTags::*)(int))&PyAxisTags::dropAxis)
		// // .def("__delitem__", (void (PyAxisTags::*)(std::string const &))&PyAxisTags::dropAxis)
		// // .def("__len__", &PyAxisTags::size)
		// // .def(self == self)
		// // .def(self != self)
		// // .def("axisTypeCount", &PyAxisTags::axisTypeCount)
		// // .def("axesByFlag", &PyAxisTags::axesByFlag)
		// // .def("spatialAxes", &PyAxisTags::spatialAxes)
		// // .def("temporalAxes", &PyAxisTags::temporalAxes)
		// // .def("channelAxes", &PyAxisTags::channelAxes)
		// // .def("frequencyAxes", &PyAxisTags::frequencyAxes)
		// // .def("angularAxes", &PyAxisTags::angularAxes)
		// // .def("untaggedAxes", &PyAxisTags::untaggedAxes)
		// // .def("canonicalOrdering", &PyAxisTags::canonicalOrdering)
		// // .def("matchOrdering", &PyAxisTags::matchOrdering)
		// // .def("transpose", (void (PyAxisTags::*)(object const &))&PyAxisTags::transpose)
		// // .def("transpose", (void (PyAxisTags::*)())&PyAxisTags::transpose)
		// // .def("transform", &PyAxisTags_transform)
    // // ;
    
    // // def("printAxistags", &printAxistags);
}

} // namespace vigra
