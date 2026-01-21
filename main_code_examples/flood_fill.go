package main

import "fmt"

func fill(grid *[10][10]int, r int, c int, oldColor int, newColor int) {
    if r < 0 || r >= 10 || c < 0 || c >= 10 {
        return
    }
    if (*grid)[r][c] != oldColor {
        return
    }
    (*grid)[r][c] = newColor
    fill(grid, r+1, c, oldColor, newColor)
    fill(grid, r-1, c, oldColor, newColor)
    fill(grid, r, c+1, oldColor, newColor)
    fill(grid, r, c-1, oldColor, newColor)
}

func main() {
    var grid = [10][10]int{
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}
    var i int = 0

    var startRow int
    var startCol int
    var newColor int
    fmt.Scan(&startRow, &startCol, &newColor)

    if startRow < 0 || startRow >= 10 || startCol < 0 || startCol >= 10 {
        fmt.Print("start out of range\n")
        return
    }

    var oldColor int = grid[startRow][startCol]
    if oldColor == newColor {
        fmt.Print("no changes\n")
        return
    }

    fill(&grid, startRow, startCol, oldColor, newColor)

    i = 0
    for i < 10 {
        var j2 int = 0
        for j2 < 10 {
            fmt.Print(grid[i][j2], " ")
            j2 = j2 + 1
        }
        fmt.Print("\n")
        i = i + 1
    }
}
