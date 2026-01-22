package main

import "fmt"

func main() {
	var n int
	fmt.Print("Enter n: ")
	fmt.Scan(&n)
	if n <= 0 {
		fmt.Print("invalid n")
		return
	}

	var data [10]int
	if n > 10 {
		fmt.Print("too big")
		return
	}
	for i := 0; i < n; i++ {
		fmt.Scan(&data[i])
	}

	var low int
	var high int
	fmt.Scan(&low, &high)
	if low < 0 || high > n || low > high {
		fmt.Print("invalid slice")
		return
	}

	part := data[low:high]
	fmt.Println(data)
	fmt.Println(part)
	part[low] *= 8
	fmt.Println(data)
	fmt.Println(part)
}
