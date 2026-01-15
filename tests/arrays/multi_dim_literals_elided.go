package main

func main() {
    arr := [2][3]int{{1, 2, 3}, {4, 5, 6}}
    _ = arr[0][2]

    slice := [][]int{{1, 2, 3}, {4, 5, 6}}
    _ = slice[1][0]
}
