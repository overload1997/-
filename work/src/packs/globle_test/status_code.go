package main

var Code = &ResponCode{}
func CodeInit() {
	Code.Success=0
	Code.PswErr=11
	Code.AccNone=10
	Code.GetIdtErr=20
	Code.UpdateDataErr=30
	Code.SidErr=41
	Code.SidNone=42
	Code.SidOverdue=40
	Code.AccExit=50 
	Code.PswFormatErr=51
	Code.IdtCodeErr=52
	Code.DatabaseErr=-1
	Code.IdtCodeOverdue=53
}

var Message = &ResponMessage{}
func MessageInit() {
	Message.Success = "成功"
	Message.PswErr = "登录密码错误"
	Message.AccNone = "帐号不存在"
	Message.IdtfCodeErr = "获取验证码失败(手机号错误或者系统崩溃)"
	Message.UpdateDataErr = "更新的数据不合法"
	Message.SidErr  = "SessonId 错误"
	Message.SidNone  = "SessonId 不存在，清重新登录"
	Message.SidOverdue  = "SessonId 过期，请重新登录"
	Message.AccExit  = "帐号已存在"
	Message.PswFormatErr  = "密码不合法"
	Message.IdtCodeErr  = "验证码不正确"
	Message.IdtCodeOverdue  = "验证码过期"
	Message.DatabaseErr = "数据库出错"
	Message.IdtCodeOverdue = "验证码过期"
}

type ResponCode struct {
	Success int
	PswErr int //登录密码错误
	AccNone int //帐号不存在
	GetIdtErr int//获取验证码失败(手机号错误或者系统崩溃)
	UpdateDataErr int//更新的数据不合法
	SidErr  int //SessonId 错误
	SidNone  int //SessonId 不存在，清重新登录
	SidOverdue  int //SessonId 过期，请重新登录
	AccExit  int //帐号已存在
	PswFormatErr  int //密码不合法
	IdtCodeErr  int //验证码不正确
	DatabaseErr int //数据库出错
	IdtCodeOverdue int //验证码过期
}

type ResponMessage struct {
	Success string
	PswErr string //登录密码错误
	AccNone string //帐号不存在
	IdtfCodeErr string //获取验证码失败(手机号错误或者系统崩溃)
	UpdateDataErr string //更新的数据不合法
	SidErr  string //SessonId 错误
	SidNone  string //SessonId 不存在，清重新登录
	SidOverdue  string //SessonId 过期，请重新登录
	AccExit  string //帐号已存在
	PswFormatErr  string //密码不合法
	IdtCodeErr  string //验证码不正确
	DatabaseErr string //数据库出错
	IdtCodeOverdue string //验证码过期
}