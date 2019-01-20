template<class T>
const T& Min(const T& left, const T& right)
{
    return left <= right ? left : right;
}
