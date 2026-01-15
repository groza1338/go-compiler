package main

func main() {
    arr := [2][3]int{[3]int{1, 2, 3}, [3]int{4, 5, 6}}
    _ = arr[1][2]

    slice := [][]int{[]int{1, 2, 3}, []int{4, 5, 6}}
    _ = slice[0][1]
}
