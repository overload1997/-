package main

import (
	"database/sql"
	"fmt"
	"log"
	"sort"
)

type farmiliar_obj struct {
	id int
	score float64
}

var ReadBook map[string]bool = make(map[string]bool)
var Farmiliar map[int]([]farmiliar_obj) = make(map[int]([]farmiliar_obj))

type farmiliar_obj_slice []farmiliar_obj

func (a farmiliar_obj_slice) Len() int {    // 重写 Len() 方法
    return len(a)
}
func (a farmiliar_obj_slice) Swap(i, j int){     // 重写 Swap() 方法
    a[i], a[j] = a[j], a[i]
}
func (a farmiliar_obj_slice) Less(i, j int) bool {    // 重写 Less() 方法， 从大到小排序
    return a[i].score > a[j].score
}

//use GetDistSecondVersion

var farmiliar_count int = 10

func HaveBorrow(stu_id string) {
	db,_:=sql.Open(SqlDriver,SqlSourceName)
	defer db.Close()
	//println("尝试连接数据库......")
	if err:=db.Ping();err!=nil {
		log.Fatal(err)
		return
	}
	//println("数据库连接成功!")
	rows,query_err := db.Query("select book_name from book_borrow_history where stu_id=\""+stu_id+"\" limit 2")
	if query_err!=nil {
		log.Fatal(query_err)
		return
	}
	for rows.Next() {
		var book_name string
		rows.Scan(&book_name)
		_,ok := ReadBook[book_name]
		if ok==false {
			fmt.Printf("%+v\n", book_name)
			ReadBook[book_name]=true
		}
		
	}
}

func GetFarmiliar() {
	fmt.Println("接下来将运行协同过滤算法")
	for i:=0;i<clustnum;i++ {
		clust:=ClustArr[i]
		for stud1,_:=range clust.clust_set.mp {
			for stud2,_:=range clust.clust_set.mp {
				if stud1 == stud2 {
					continue
				}
				fml_obj:=farmiliar_obj{stud2,GetDistSecondVersion(stud1,stud2)}
				Farmiliar[stud1] = append(Farmiliar[stud1], fml_obj)
			}
			sort.Sort(farmiliar_obj_slice(Farmiliar[stud1]))
			if UserName[stud1] != "1505100008" {
				continue
			}
			fmt.Printf("跟学生%v兴趣最相似的%d个人是:\n",UserName[stud1],farmiliar_count)
			var output_count int = 0
			for i:=0;i<len(Farmiliar[stud1]);i++ {
				stud_obj := Farmiliar[stud1][i]
				stud2 := stud_obj.id
				fmt.Printf("%v ", UserName[stud2])
				output_count++
				if output_count >= farmiliar_count {
					break
				}
			}
			output_count = 0
			fmt.Printf("\n\n给学生%v推荐的书籍有:\n",UserName[stud1])
			for stud2,_:=range clust.clust_set.mp {
				output_count++
				if output_count >= farmiliar_count {
					break
				}
				HaveBorrow(UserName[stud2])
			}
			fmt.Println()
		}
	}
}

