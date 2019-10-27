// Oðuz Kerem Tural
// 150130125
// modified by B. Brzózka

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <map>
#include <iomanip>
#include <QTextStream>
#include <QMessageBox>
#include <QPdfWriter>
#include <QFileDialog>
#include <QDateTime>
#include <QTextDocument>
#include <QFile>
#include <QDesktopServices>
#include "NeedlemanWunsch.h"



NeedlemanWunsch::NeedlemanWunsch(std::string dna_a, std::string dna_b)
{
	this->_dna_a = dna_a;
	this->_dna_b = dna_b;

	this->_length_of_a = dna_a.length();
	this->_length_of_b = dna_b.length();

	this->_similarity = 0;

	this->_similarity_matrix = new int*[this->_length_of_a + 2];
    for (size_t i = 0; i < (this->_length_of_a + 1); i++)
	{
		this->_similarity_matrix[i] = new int[this->_length_of_b + 2];
	}

    for (size_t i = 0; i < _length_of_a; i++)
	{
        for (size_t j = 0; j < _length_of_a; j++)
		{
			_similarity_matrix[i][j] = 0;
		}
	}
}

NeedlemanWunsch::~NeedlemanWunsch()
{
    for (size_t i = 0; i < (_length_of_a + 2); i++)
	{
		delete[] this->_similarity_matrix[i];
	}
	delete[] this->_similarity_matrix;
}

// Calculating similarity matrix
void NeedlemanWunsch::calculate_similarity()
{
	enum AminoAcid amino = MINSCORE;
	int gap_penalty = _subsitition_matrix[0][amino];
    for (size_t i = 0; i < (_length_of_a + 1); i++)
	{
        _similarity_matrix[i][0] = int (i) * gap_penalty;
	}

    for (size_t j = 0; j < (_length_of_a + 1); j++)
	{
        _similarity_matrix[0][j] = int(j) * gap_penalty;
	}

	int match = 0, insert = 0, del = 0 , max = 0, selected = 0;
    for (size_t i = 1; i < _length_of_a + 1; i++)
	{
        for (size_t j = 1; j < _length_of_b + 1; j++)
		{
			match = _similarity_matrix[i - 1][j - 1] + 
				_subsitition_matrix[_dna_int_map[_dna_a[i - 1] ] ][_dna_int_map[_dna_b[j - 1] ] ];
			del = _similarity_matrix[i - 1][j] + gap_penalty;
			insert = _similarity_matrix[i][j - 1] + gap_penalty;

			max = (match < del) ? del : match;
			selected = ((max < insert) ? insert : max);

			_similarity_matrix[i][j] = selected;
		}
	}
}

// Reading BLOSUM62 matrix.
void NeedlemanWunsch::populate_subs_matrix(std::string subs_matrix_file)
{
	std::fstream subs_matrix;
	subs_matrix.open(subs_matrix_file.c_str());
	if (!subs_matrix)
	{
		std::cerr << "Error (3): Substition matrix file not found! \n";
		exit(3);
	}
	std::string line, parsed;
	int i = 0, j = 0;
	while (std::getline(subs_matrix, line) )
	{
		if (line[0] != '#')
		{
			std::istringstream iss(line);

			j = 0;
			while (std::getline(iss, parsed, ' '))
			{
				if (parsed != "") 
				{
					if (i == 0) 
					{
						_dna_int_map.insert(std::pair<char, int>(parsed[0], j));
					}
					if (j > 0 && i > 0)
					{
						_subsitition_matrix[i - 1][j - 1] = atoi(parsed.c_str());
					}
					j++;
				}
			}
			i++;
		}
	}
}

// Trace back step.
void NeedlemanWunsch::dna_align()
{
	enum AminoAcid amino = MINSCORE;
    int gap_penalty = _subsitition_matrix[0][amino];
	_alignment_a = "";
	_alignment_b = "";
	_signs = "";
    this -> _gaps=0;
    this -> _identity =0;
	size_t i = _length_of_a;
    size_t j = _length_of_b;
	while (i > 0 || j > 0)
	{
		// Going to S(i-1, j-1)
		if (i > 0 && j > 0
			&& (_similarity_matrix[i][j] ==
				_similarity_matrix[i - 1][j - 1]
				+ _subsitition_matrix[_dna_int_map[_dna_a[i - 1]]][_dna_int_map[_dna_b[j - 1]]]))
		{
			_alignment_a = _dna_a[i - 1] + _alignment_a;
			_alignment_b = _dna_b[j - 1] + _alignment_b;

			if (_subsitition_matrix[_dna_int_map[_dna_a[i - 1]]][_dna_int_map[_dna_b[j - 1]]] > 0)
			{
				if (_dna_a[i - 1] != _dna_b[j - 1])
				{
					_signs = ":" + _signs;
				}
				else
				{
					_signs = "|" + _signs;
					_identity += 1;
				}
				_similarity += 1;
			}
			else
			{
				_signs = "." + _signs;
			}

			i -= 1;
			j -= 1;
		}
		// Going to S(i-1, j)
		else if (i > 0
			&& _similarity_matrix[i][j] == (_similarity_matrix[i - 1][j] + gap_penalty))
		{
			_alignment_a = _dna_a[i - 1] + _alignment_a;
			_alignment_b = '-' + _alignment_b;
			_signs = " " + _signs;
			_gaps += 1;

			i -= 1;
		}
		// Going to S(i, j-1)
		else
		{
			_alignment_a = '-' + _alignment_a;
			_alignment_b = _dna_b[j - 1] + _alignment_b;
			_signs = " " + _signs;
			_gaps += 1;
			j -= 1;
		}
    }
}

void NeedlemanWunsch::print_results()
{
    QString text,text1;
    QDateTime t = QDateTime::currentDateTime();
    QString d = t.toString("dddd dd-MM-yyyy HH:mm:ss");
    std::cout << _similarity_matrix[10][10];
	std::cout << "\nAlignment: \n\n";
    lista.append("Raport wykonano: " + d + "\n\n\n");
    lista.append("Raport z porównania sekwencji:\n\n\n");
    lista.append("Poniższe sekwencje zawierają znaki dodane w trakcie alignmentu. \n\nSekwencja 1:\n");
	for (size_t i = 0; i < _alignment_a.length(); i++)
    {
        lista.append(QString( _alignment_a[i]));
        text.append(QString( _alignment_a[i]));
	}
    std::cout<<text.toStdString();
    QFile file("seq3.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << text;
    file.close();

    lista.append("\n\nSekwencja 2:\n");
	for (size_t i = 0; i < _alignment_b.length(); i++)
	{
        lista.append(QString(_alignment_b[i]));
        text1.append(QString(_alignment_b[i]));
    }
    std::cout << text1.toStdString();
    QFile file1("seq4.txt");
    file1.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out1(&file1);
    out1 << text1;
    file1.close();

	double percentage;
    lista.append("\n\n\nWynik: " + QString::number(_similarity_matrix[_length_of_a][_length_of_b]));
    lista.append("\nDługość:.......... " + QString::number(_alignment_a.length()) + " (z przerwami)");
    percentage = (double (_identity) / double (_alignment_a.length())) * 100;
    lista.append("\nIdentycznych:... " + QString::number(_identity) +  "/" + QString::number(_alignment_a.length()) + " (" + QString::number(percentage) + "%)");
    percentage = (double(_similarity) / double(_alignment_a.length())) * 100;
    lista.append("\nPodobnych:...... " + QString::number(_similarity) +  "/" + QString::number(_alignment_a.length()) + " (" + QString::number(percentage) + "%)");
    percentage = (double(_gaps) / double(_alignment_a.length())) * 100;
    lista.append("\nPrzerw:............ " + QString::number(_gaps) +  "/" + QString::number(_alignment_a.length()) + " (" + QString::number(percentage) + "%)");

    QFile files("pdf.txt");
    files.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream outs(&files);
    outs << lista;
    files.close();




}


