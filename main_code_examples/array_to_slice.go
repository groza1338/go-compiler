package main

import "fmt"

func main() {
	var n int
	fmt.Print("Enter n: ")
	fmt.Scan(&n)
	if n <= 0 || n > 10 {
		fmt.Print("invalid n")
		return
	}

	var data [10]int
	for i := 0; i < n; i++ {
		fmt.Scan(&data[i])
	}

	s := data[:n]
	fmt.Println(data[:n])
	fmt.Println(s)

	var low int
	var high int
	fmt.Scan(&low, &high)
	if low < 0 || high > n || low >= high {
		fmt.Print("invalid slice")
		return
	}

	part := s[low:high]
	fmt.Println(part)
	sum := 0
	limit := high - low
	for i := 0; i < limit; i++ {
		sum += part[i]
	}
	fmt.Println("sum", sum)
}
