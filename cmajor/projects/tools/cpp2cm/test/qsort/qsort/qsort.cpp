#include <iostream>
#include <random>

int Partition(std::vector<int>& intList, int left, int right)
{
    int i = left;
    int j = right;
    int pivot = intList[(left + right) / 2];
    while (i <= j)
    {
        while (intList[i] < pivot) ++i;
        while (intList[j] > pivot) --j;
        if (i <= j)
        {
            std::swap(intList[i], intList[j]);
            ++i;
            --j;
        }
    }
    return i;
}

void QuickSort(std::vector<int>& intList, int left, int right)
{
    int index = Partition(intList, left, right);
    if (left < index - 1)
    {
        QuickSort(intList, left, index - 1);
    }
    if (index < right)
    {
        QuickSort(intList, index, right);
    }
}

void QuickSort(std::vector<int>& intList)
{
    if (!intList.empty())
    {
        QuickSort(intList, 0, intList.size() - 1);
    }
}

void Print(const std::vector<int>& intList)
{
    bool first = true;
    std::cout << '[';
    for (int x : intList)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            std::cout << ", ";
        }
        std::cout << x;
    }
    std::cout << ']' << std::endl;
}

int main()
{
    std::default_random_engine re;
    std::uniform_int_distribution<> d{ 0, 100 };
    std::vector<int> intList;
    int n = 10;
    for (int i = 0; i < n; ++i)
    {
        intList.push_back(d(re));
    }
    Print(intList);
    QuickSort(intList);
    Print(intList);
    return 0;
}

