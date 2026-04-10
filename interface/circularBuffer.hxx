#include <QVector>
#include <cstddef>
#include <qcontainerfwd.h>

template<typename _T>
class CircularBuffer : public QVector<_T>
{
  public:
    CircularBuffer( size_t size ) : QVector<_T>( size, 0 ), _size( size )
    {
        QVector<_T>::shrink_to_fit();
    }

    void push_back( double value )
    {
        QVector<_T>::operator[]( _head ) = value;
        _head                            = ( _head + 1 ) % _size;
    }

    _T &operator[]( size_t index )
    {
        return QVector<_T>::operator[]( ( _head + index ) % _size );
    }

    const _T &operator[]( size_t index ) const
    {
        return this->operator[]( index );
    }

    // const QVector<double> &data() const
    // {
    //     return _buffer;
    // }

  private:
    size_t _size;
    size_t _head { 0 };
};
