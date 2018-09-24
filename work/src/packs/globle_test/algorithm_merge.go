package main

import (
	"log"
	"fmt"
	"math"
	"database/sql"
)

var bookid int
var userid int
var clustnum int
var itertimes int = 10
var book_be_read map[int]int = make(map[int]int)

type stu_type_obj struct{
	stu_id string
	book_type string
}

type User struct {
	user_name string;
	book_set int;
}

func solve_err(err error) {
	if err!=nil {
		panic(err)
	}
}

func NewUser (name string) User{
	new_user := new(User)
	new_user.user_name=name;
	new_user.book_set=0; 
	return *new_user
}

//create a struct of set (int)
type Set struct {
	mp map[int]bool
	size int
}

func NewSet() Set{
	return Set{map[int]bool{},0}
}

func (s *Set) Size() int {
	return s.size;
}

func (s *Set) Insert(key int) {
	_,exist:=s.mp[key]
	if exist==false {
		s.mp[key]=true
		s.size++
	}
}

func (s *Set) Erase(key int) {
	_,exist:=s.mp[key]
	if exist==true {
		delete(s.mp,key)
		s.size--
	}
}

func (s *Set) Find(key int) bool {
	_,ok := s.mp[key]
	return ok
}

func (s *Set) Empty(key int) bool {
	return s.size==0
}

var UserArr []User

type Clust struct {
	status int
	clust_set Set	
}
func (clust *Clust) GetClustStatus() {
	var ans int = 0
	var cnt [32]int
	for i:=0;i<32;i++ {
		cnt[i]=0
	}
	for iter,_:=range clust.clust_set.mp {
		for i:=uint(0);i<32;i++ {
			if (1<<i)&UserArr[iter].book_set > 0 {
				cnt[i]++
			}
		}
	}
	for i:=uint(0);i<32;i++ {
		if cnt[i]*2 >= clust.clust_set.Size() {
			ans=ans|(1<<i)
		}
	}
	clust.status=ans
}

var UserId map[string]int = make(map[string]int)
var UserName map[int]string = make(map[int]string)
var ClustArr []Clust
var BookId map[string]int = make(map[string]int)
var Belong map[int]int = make(map[int]int)

func CountBit(x int) int{
	var ans int = 0
	for ;x>0;ans++ {
		x&=x-1
	}
	return ans
}

func GetSqrt(x float64) float64 {
	var eps float64 = 0.0000001
	left:=0.0
	right:=x
	var mid float64
	for ;math.Abs(right-left)>eps; {
		mid=left+(right-left)/2.0
		if mid*mid>x {
			right=mid
		} else {
			left=mid
		}
	}
	return mid
}

func GetDistFirstVersion(x int,y int) float64 {
	A:=x|y
	B:=x&y
	return float64(CountBit(B))/GetSqrt(float64(CountBit(A)))
}

func GetDistSecondVersion(x int,y int) float64 {
	var ans float64 = 0.0
	var same_book_set int = UserArr[x].book_set & UserArr[y].book_set
	for i:=uint(0);i<32;i++ {
		if same_book_set&(1<<i)!=0  {
			ans+=1.0/(math.Log(1)+float64(book_be_read[int(i)]))
		}
	}
	ans/=float64(CountBit(UserArr[x].book_set))
	return ans
}

func Init() {
	userid = 0
	bookid = 0
}

func ReadBorrowInfo() {
	//connect database
	db,_:=sql.Open(SqlDriver,SqlSourceName)
	defer db.Close()
	println("尝试连接数据库......")
	if err:=db.Ping();err!=nil {
		log.Fatal(err)
		return
	}
	println("数据库连接成功!")
	println("尝试获取图书借阅信息......")
	rows,db_err:=db.Query("select stu_id,type from book_borrow_history,book_info where book_borrow_history.book_name=book_info.title")
	solve_err(db_err)
	println("获取图书借阅信息成功!")
	borrow_obj:=stu_type_obj{}
	println("预处理借书信息......")
	for rows.Next() {
		scan_err := rows.Scan(&borrow_obj.stu_id,&borrow_obj.book_type)
		solve_err(scan_err)
		_,user_exit := UserId[borrow_obj.stu_id]
		if user_exit == false {
			UserId[borrow_obj.stu_id]=userid
			UserName[userid]=borrow_obj.stu_id
			userid++
			new_user:=NewUser(borrow_obj.stu_id)
			UserArr = append(UserArr, new_user)
		}
		_type:=borrow_obj.book_type
		//check if invalid
		if _type=="" {
			continue
		}
		var current_user_id int = UserId[borrow_obj.stu_id]
		var book_type int = int(_type[0]-'A')
		if UserArr[current_user_id].book_set & 1<<uint(book_type) == 0 {
			book_be_read[book_type]++
			UserArr[current_user_id].book_set |= 1<<uint(book_type)
		}
		//fmt.Printf("%d %d\n", current_user_id,book_type)
	}
	println("预处理借书信息成功!接下来即将运行聚类算法......")
}

func Debug() {
	for i:=0;i<clustnum;i++ {
		println("\n簇",i,"内的学生学号:")
		Clust_i:=ClustArr[i]
		for j,_:=range Clust_i.clust_set.mp {
			fmt.Printf(UserName[j]+" ")
		}
	}
}

func GetClust() {
	fmt.Printf("学生总人数:%d\n", userid)
	clustnum=int(GetSqrt(float64(userid)))
	fmt.Printf("簇总数%+v\n", clustnum)
	for i:=0;i<clustnum;i++ {
		var SET Set = NewSet()
		SET.Insert(i)
		ClustArr = append(ClustArr, Clust{UserArr[i].book_set,SET})
		Belong[i]=i
	}
	for times:=0;times<itertimes;times++ {
		for i:=0;i<userid;i++ {
			var nearest_clust_id int = -1
			var nearest_clust_dis float64 = 0.0
			for j:=0;j<clustnum;j++ {
				var dist float64=GetDistFirstVersion(UserArr[i].book_set,ClustArr[j].status)
				//fmt.Printf("%+v\n", dist)
				if dist>nearest_clust_dis {
					nearest_clust_dis = dist
					nearest_clust_id = j
				}
			}
			_,ok := Belong[i]
			if ok && Belong[i]==nearest_clust_id {
				continue;
			}
            if ok && ClustArr[Belong[i]].clust_set.Size()==1 {
				 continue;
			}
			_,exist:=Belong[i] 
			if exist==true {
				ClustArr[Belong[i]].clust_set.Erase(i);
			}
			//fmt.Printf("%d ", nearest_clust_id)
			if nearest_clust_id!=-1 {
            	ClustArr[nearest_clust_id].clust_set.Insert(i);
				Belong[i]=nearest_clust_id;
			}
		}
		for i:=0;i<clustnum;i++ {
			//reset the quarity medium
			ClustArr[i].GetClustStatus()
		}
	}
	fmt.Println("Get clust Finished!")
}

func Merge() {
	Init()
	ReadBorrowInfo()
	GetClust()
	//Debug()
	//调用协同过滤算法
	GetFarmiliar()
}