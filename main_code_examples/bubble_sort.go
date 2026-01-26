package main

import "fmt"

func main() {
	var n int
	fmt.Print("Enter array len: ")
	fmt.Scan(&n)
	const array_len int = 100
	if n > array_len || n < 1 {
	    fmt.Print("Incorrect len")
	    return
	}
	arr := [array_len]int{}
	for x := 0; x < n; x++ {
	    fmt.Print("Введите", x,"-ый элемент массива: ")
		fmt.Scan(&arr[x])
	}
	for i := 0; i < n; i++ {
		for j := 0; j < n-1; j++ {
			if arr[j] > arr[j+1] {
				tmp := arr[j]
				arr[j] = arr[j+1]
				arr[j+1] = tmp
			}
		}
	}

    fmt.Print(arr[:n])
}