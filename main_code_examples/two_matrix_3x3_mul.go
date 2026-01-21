package main

import "fmt"

func main() {
	var matrix1 [3][3]int
	var matrix2 [3][3]int
	var result [3][3]int

	fmt.Println("Введите элементы первой матрицы 3x3:")
	for i := 0; i < 3; i++ {
		for j := 0; j < 3; j++ {
			fmt.Print("Введите элемент matrix1: ")
			fmt.Scan(&matrix1[i][j])
		}
	}

	fmt.Println("Введите элементы второй матрицы 3x3:")
	for i := 0; i < 3; i++ {
		for j := 0; j < 3; j++ {
			fmt.Print("Введите элемент matrix2: ")
			fmt.Scan(&matrix2[i][j])
		}
	}

	for i := 0; i < 3; i++ {
		for j := 0; j < 3; j++ {
			result[i][j] = 0
			for k := 0; k < 3; k++ {
				result[i][j] += matrix1[i][k] * matrix2[k][j]
			}
		}
	}

	fmt.Println("Результат перемножения матриц:")
	for i := 0; i < 3; i++ {
		for j := 0; j < 3; j++ {
			fmt.Print(result[i][j])
		}
		fmt.Println()
	}
}
