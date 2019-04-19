
#ifndef MANUPROC_PROPERTY
#define MANUPROC_PROPERTY

#include "MpcSignal.hh"

#include <iostream>

// A MpcProperty is encpsulates a value and may inform
// you on any changes applied to this value.

template <typename T> class MpcProperty 
{

public:
  // The property value type
  typedef T value_type;

  // Properties for built-in types are automatically
  // initialized to 0. See template spezialisations
  // at the bottom of this file

  MpcProperty()
    : connection_(nullptr)
      , connection_id_(-1)
 {}

  MpcProperty(T const& val)
    : value_(val)
      , connection_(nullptr)
      , connection_id_(-1)
  {}

  MpcProperty(T&& val)
    : value_(std::move(val))
      , connection_(nullptr)
      , connection_id_(-1)
  {}

  MpcProperty(MpcProperty<T> const& to_copy)
    : value_(to_copy.value_)
      , connection_(nullptr)
      , connection_id_(-1)
  {}

  MpcProperty(MpcProperty<T>&& to_copy)    
    : value_(std::move(to_copy.value_))
      , connection_(nullptr)
      , connection_id_(-1)
  {}


  // returns a Signal which is fired when the internal value
  // will be changed. The old value is passed as parameter.
  virtual MpcSignal<T> const& before_change() const
  { 
    return before_change_; 
  }

  // returns a Signal which is fired when the internal value
  // has been changed. The new value is passed as parameter.
  virtual MpcSignal<T> const& on_change() const
  { 
    return on_change_; 
  }

  // sets the MpcProperty to a new value. before_change() and
  // on_change() will be emitted.
  virtual void set(T const& value)
  {
    if (value != value_) 
    {
      before_change_.emit(value_);
      value_ = value;
      on_change_.emit(value_);
    }
  }

  // sets the MpcProperty to a new value. before_change() and
  // on_change() will not be emitted
  void set_with_no_emit(T const& value)
  {
    value_ = value;
  }

  // emits before_change() and on_change() even if the value
  // did not change
  void touch()
  {
      before_change_.emit(value_);
      on_change_.emit(value_);
  }

  // returns the internal value
  virtual T const& get() const
  { 
    return value_; 
  }

  // connects two Properties to each other. If the source's
  // value is changed, this' value will be changed as well
  virtual void connect_from(MpcProperty<T> const& source)
  {
    disconnect();
    connection_ = &source;
    connection_id_ = source.on_change().connect
        ([this](T const& value)
          {
            set(value);
            return true;
          }
        );
    set(source.get());
  }

  // if this MpcProperty is connected from another property,
  // it will e disconnected
  virtual void disconnect()
  {
    if (connection_) 
    {
      connection_->on_change().disconnect(connection_id_);
      connection_id_ = -1;
      connection_ = nullptr;
    }
  }

  // if there are any Properties connected to this Property,
  // they won't be notified of any further changes
  virtual void disconnect_auditors()
  {
    on_change_.disconnect_all();
    before_change_.disconnect_all();
  }

  // assigns the value of another Property
  virtual MpcProperty<T>& operator=(MpcProperty<T> const& rhs)
  {
    set(rhs.value_);
    return *this;
  }

  // assigns a new value to this Property
  virtual MpcProperty<T>& operator=(T const& rhs)
  {
    set(rhs);
    return *this;
  }

  // compares the values of two Properties
  bool operator==(MpcProperty<T> const& rhs) const
  {
    return MpcProperty<T>::get() == rhs.get();
  }

  bool operator!=(MpcProperty<T> const& rhs) const
  {
    return MpcProperty<T>::get() != rhs.get();
  }

  // compares the values of the MpcProperty to another value
  bool operator==(T const& rhs) const
  { 
  return MpcProperty<T>::get() == rhs; 
  }

  bool operator!=(T const& rhs) const
  { 
    return MpcProperty<T>::get() != rhs; 
  }

  // returns the value of this MpcProperty
  T const& operator()() const
  { 
    return MpcProperty<T>::get(); 
  }

private:
  // Sends signal on change of this property
  MpcSignal<T> on_change_;

  // Sends signal when just going to change this property
  MpcSignal<T> before_change_;

  // Connection to chain the properties 
  MpcProperty<T> const* connection_;
  int connection_id_;

  // The value of the property
  T value_;
};

// specialization for built-in default contructors
template<> inline MpcProperty<double>::MpcProperty()
  : connection_(nullptr), connection_id_(-1), value_(0.0) {}

template<> inline MpcProperty<float>::MpcProperty()
  : connection_(nullptr), connection_id_(-1), value_(0.f) {}

template<> inline MpcProperty<short>::MpcProperty()
  : connection_(nullptr), connection_id_(-1), value_(0) {}

template<> inline MpcProperty<int>::MpcProperty()
  : connection_(nullptr), connection_id_(-1), value_(0) {}

template<> inline MpcProperty<char>::MpcProperty()
  : connection_(nullptr), connection_id_(-1), value_(0) {}

template<> inline MpcProperty<unsigned>::MpcProperty()
  : connection_(nullptr), connection_id_(-1), value_(0) {}

template<> inline MpcProperty<bool>::MpcProperty()
  : connection_(nullptr), connection_id_(-1), value_(false) {}

// stream operators
template<typename T>
std::ostream& operator<<(std::ostream& out_stream, MpcProperty<T> const& val) 
{
  out_stream << val.get();
  return out_stream;
}

template<typename T>
std::istream& operator>>(std::istream& in_stream, MpcProperty<T>& val) 
{
  T tmp;
  in_stream >> tmp;
  val.set(tmp);
  return in_stream;
}

// stream operators
template<typename T> std::ostream& operator<<(std::ostream& out_stream, MpcProperty<T> const& val) ;
template<typename T> std::istream& operator>>(std::istream& in_stream, MpcProperty<T>& val);

#endif // MANUPROC_PROPERTY
