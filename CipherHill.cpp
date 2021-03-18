#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>
#define LenABC 53

using namespace std;

struct algEvclida
{
	int nod, x;
};

void ReadFile(ifstream& in, int mode, vector<char>* ABC);
string EncodeString(string EnString, vector<int> KeyMat, vector<char>* ABC);
vector<int> BuildMat(string InString, int dimension, vector<char>* ABC);
string MatInString(vector<int> Mat, vector<char>* ABC);
string DecodeString(string DeString, vector<int> KeyMat, vector<char>* ABC);
algEvclida MCD(int a, int b);
int Determinant(vector<int> Mat);
int Determinant2Mat(vector<int> Mat);
vector<int> MatAlgDop(vector<int> Mat);
vector<int> MatMinor(vector<int> Mat, int colum, int row);

int main()
{
	vector<char> ABC = { 'A','S','D','F','G','H','J','K','L','Q',
		'W','E','R','T','Y','U','I','O','P','Z','X','C','V','B',
		'N','M','a','s','d','f','g','h','j','k','l','q','w','e',
		'r','t','y','u','i','o','p','z','x','c','v','b','n','m',' ' };

	string NameInFile;
	cout << "Input file with text: ";
	cin >> NameInFile;

	ifstream fin(NameInFile);
	if (!fin)
	{
		cout << "Unable to open the file." << endl;
		return 1;
	}
	ReadFile(fin, 0, &ABC);
	fin.close();

	cout << "Input file with cipher: ";
	cin >> NameInFile;
	fin.open(NameInFile);
	if (!fin)
	{
		cout << "Unable to open the file." << endl;
		return 1;
	}

	ReadFile(fin, 1, &ABC);

	fin.close();

	return 0;
}

//Чтение данных из файла
void ReadFile(ifstream& in, int mode, vector<char>* ABC)
{
	string Key;
	ofstream out, outBin;

	cout << "Key for encode: ";
	cin >> Key;
	vector<int> KeyMat = BuildMat(Key, 0, ABC);

	if (mode == 0)
	{
		out.open("EnccodeFileOut.txt");
	}
	else if (mode == 1)
	{
		out.open("DeccodeFileOut.txt");
	}

	string buffer;
	if (in)
	{
		while (in >> buffer)
		{
			if (mode == 0)
			{
				cout << EncodeString(buffer, KeyMat, ABC) << " ";
				out << EncodeString(buffer, KeyMat, ABC) << " ";
			}
			else if (mode == 1) cout << DecodeString(buffer, KeyMat, ABC) << " ";
			cout << endl;
		}
	}

	out.close();
}

//Функция кодировки сообщения
string EncodeString(string EnString, vector<int> KeyMat, vector<char>* ABC)
{
	int CapasityKeyMat = pow(KeyMat.size(), 0.5); //Размерность матрицы ключа
	vector<int> EnStringMat = BuildMat(EnString, CapasityKeyMat, ABC); //Построение матрицы слова
	int CapasityEnStringMat = EnStringMat.size() / CapasityKeyMat; //Размерность матрицы слова
	vector<int> OutStringMat; //Итоговая матрица

	for (int i = 0; i < CapasityEnStringMat; i++)
		for (int j = 0; j < CapasityKeyMat; j++)
		{
			int cum = 0;
			for (int k = 0; k < CapasityKeyMat; k++)
			{
				cum += EnStringMat[i * CapasityKeyMat + k] * KeyMat[k * CapasityKeyMat + j]; //Умножение матриц
			}
			OutStringMat.push_back(cum % LenABC);
		}
	return MatInString(OutStringMat, ABC);
}

//Постоение матрицы из слова
vector<int> BuildMat(string InString, int dimension, vector<char>* ABC)
{
	int sizeKey = InString.size(), sizeMat = sizeKey;
	//Определение размерности матрицы ключа
	if (dimension == 0)
	{
		float root = pow(sizeMat, 0.5);
		if (root == ceil(root) && root >= 2) {}
		else sizeMat = pow(floor(root) + 1, 2);
	}
	else
	{
		if (sizeMat % dimension == 0) {}
		else sizeMat = (sizeMat / dimension + 1) * dimension;
	}
	vector<int> KeyMat(sizeMat, find(ABC->begin(), ABC->end(), ' ') - ABC->begin()); 
	for (int i = 0; i < sizeKey; i++) KeyMat[i] = find(ABC->begin(), ABC->end(), InString[i]) - ABC->begin();
	return KeyMat;
}

//Преобразование матрицы в строку
string MatInString(vector<int> Mat, vector<char>* ABC)
{
	string OutMat = "";
	for (int i = 0, sizeMat = Mat.size(); i < sizeMat; i++)
	{
		OutMat += ABC->at(Mat[i]);
	}
	return OutMat;
}

//Функция декодировки сообщения
string DecodeString(string DeString, vector<int> KeyMat, vector<char>* ABC)
{
	int CapasityKeyMat = pow(KeyMat.size(), 0.5);
	int detKey = Determinant(KeyMat);
	algEvclida Evclid = MCD(abs(detKey), LenABC);
	vector<int> AlgDopKey = MatAlgDop(KeyMat);

	//Транспонирование
	for (int i = 0; i < CapasityKeyMat; i++)
		for (int j = i + 1; j < CapasityKeyMat; j++)
		{
			swap(AlgDopKey[i * CapasityKeyMat + j], AlgDopKey[j * CapasityKeyMat + i]);
		}

	
	int BackDet = 0;
	if (Evclid.x > 0) BackDet = Evclid.x;
	else if (detKey > 0) BackDet = LenABC + Evclid.x;
	else BackDet = -Evclid.x;
	
	for (int i = 0, sizeMat = AlgDopKey.size(); i < sizeMat; i++)
	{
		AlgDopKey[i] = AlgDopKey[i] * BackDet % LenABC;	
	}
	
	for (int i = 0; i < CapasityKeyMat; i++)
		for (int j = 0; j < CapasityKeyMat; j++)
		{
			if (AlgDopKey[i * CapasityKeyMat + j] < 0)
			{
				AlgDopKey[i * CapasityKeyMat + j] = LenABC + AlgDopKey[i * CapasityKeyMat + j];
			}
		}

	//Дешифровка
	vector<int> DecodeStringMat = BuildMat(DeString, CapasityKeyMat, ABC);
	int CapasityDecodeStringMat = DecodeStringMat.size() / CapasityKeyMat; //Размерность матрицы слова
	vector<int> OutStringMat; //Итоговая матрица

	for (int i = 0; i < CapasityDecodeStringMat; i++)
		for (int j = 0; j < CapasityKeyMat; j++)
		{
			int cum = 0;
			for (int k = 0; k < CapasityKeyMat; k++)
			{
				cum += DecodeStringMat[i * CapasityKeyMat + k] * AlgDopKey[k * CapasityKeyMat + j]; //Умножение матриц
			}
			OutStringMat.push_back(cum % LenABC);
		}

	return MatInString(OutStringMat, ABC);
}

//Алгоритм Евклида
algEvclida MCD(int a, int b)
{
	algEvclida U = { a, 1 }, V = { b, 0 };
	while (V.nod != 0)
	{
		int q = U.nod / V.nod;
		algEvclida T = { U.nod % V.nod, U.x - q * V.x };
		U = V;
		V = T;
	}
	return U;
}

//Нахождение определителя матрицы любого порядка
int Determinant(vector<int> Mat)
{
	int sizeMat = Mat.size(), capasityMat = pow(sizeMat, 0.5); //Размерность матрицы
	if (sizeMat == 4) return Determinant2Mat(Mat); // Для матрицы 2 порядка
	else if (sizeMat != 1)
	{
		int sum = 0;
		for (int i = 0; i < capasityMat; i++)
		{
			sum += Mat[i] * pow(-1, i) * Determinant(MatMinor(Mat, i, 0)); //Подсчет определителя матрицы
		}
		return sum;
	}
	else return Mat[0]; //Для матрицы 1 порядка
}

//Детерминант матрицы 2 порядка
int Determinant2Mat(vector<int> Mat)
{
	return Mat[0] * Mat[3] - Mat[1] * Mat[2];
}

//Нахождение матрицы алгебраического дополнения
vector<int> MatAlgDop(vector<int> Mat)
{
	vector<int> AlgDop;
	int CapasityMat = pow(Mat.size(), 0.5);
	for (int i = 0; i < CapasityMat; i++)
		for (int j = 0; j < CapasityMat; j++)
		{
			AlgDop.push_back(pow(-1, i + j) * Determinant(MatMinor(Mat, j, i)));
		}
	return AlgDop;
}

vector<int> MatMinor(vector<int> Mat, int colum, int row)
{
	int sizeMat = Mat.size(), capasityMat = pow(sizeMat, 0.5);
	vector<int> buffer = Mat; //Вектор минора

	//Удаляем строку
	buffer.erase(buffer.begin() + row * capasityMat, buffer.begin() + (row + 1) * capasityMat);

	//Удаляем столбцы
	for (int j = capasityMat - 2; j >= 0; j--)
	{
		buffer.erase(buffer.begin() + j * capasityMat + colum);
	}

	return buffer;
}