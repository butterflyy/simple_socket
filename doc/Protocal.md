# 通讯协议文档 V1.0
服务器和和客户端交互的通讯协议文档，通讯方式可以参考websocket，连接方式为TCP长连接，数据通过json来进行交互，一个请求对应一个返回。


## 请求参数说明

|  名称  | 必填 | 类型 | 说明 |
|------ |----- |----- |----- |
|   mod        | Y | string | 请求方法，例如"get_device_info" |
|   session_id | Y | string | 唯一会话 ID，每次请求都不一样，请求后返回。 |
|   type       | N | int | 类型，请求时为1 |

## 返回参数说明

|  名称  | 必填 | 类型 | 说明 |
|------ |----- |----- |----- |
|   mod        | Y | string | 返回请求的方法，例如"get_device_info" |
|   session_id | Y | string | 返回请求的会话ID。 |
|   type       | N | int | 类型，返回时为2 |
|   code       | Y | int | 错误码 |
|   message    | Y | string | 错误码的描述 |

## 错误码说明

|  错误码 | 说明 |
|------ |----- |
|   0        | 成功  |
|   -1       | 一般错误 |
|   -2       | 设备故障 |
|   -3       | 数据库异常|

## 接口列表



### 获取设备信息
* 请求顺序
	服务器 -> 客户端

* 请求JSON示例
  
             {
                 "mod":"set_device_info",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":1
             }

* 返回JSON示例

             {
                 "mod":"set_device_info",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":2,
                 "code":0,
                 "message":"",
                 "device_info":
	    			{
	    			"uuid":"31231412332",
	    			"device_ip":"192.168.1.156",
	    			"device_port":1545,
	    			"server_ip":"192.168.1.166",
	    			"server_port":49003,
	    			"address":"公司门口",
	    			"community_code":"1312312",
	    			"software_version":"V3.2.1",
	    			"hardware_version":"V5.8.9"
	    		 	}
             }
         
### 设置设备信息
* 请求顺序
	服务器 -> 客户端

* 请求JSON示例
  
             {
                 "mod":"set_device_info",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":1,
                 "device_info":
	    			{
	    			"uuid":"123119823234342"
	    			"device_ip":"192.168.1.156",
	    			"device_port":1545,
	    			"server_ip":"192.168.1.166",
	    			"server_port":39877,
	    			"address":"门口考勤机器",
	    			"community_code":"1312123312",
	    		 	}
             }

* 返回JSON示例

             {
                 "mod":"set_device_info",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":2,
                 "code":0,
                 "message":""
             }
         
### 下载人员
* 请求顺序
	服务器 -> 客户端

* 请求JSON示例
  
             {
                 "mod":"download_person",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":1,
                 "person_info":
	    			{
	    			"person_id":"3413123123"
	    			"person_name":"张飒",
	    			"id_number":“1545123",
	    			"card_number":"123123123",
	    			"key_number":“1231233”,
	    			"department_number":"111111111222",
	    			"role":12,
	    			“face”:
	    				{
	    				"width":520,
	    				"height":690,
	    				"format":2,
	    				"length":123221,
	    				"data":"BASE64 face data"
	    				}
	    			"left_image":
	    				{
	    				"width":520,
	    				"height":690,
	    				"format":2,
	    				"length":123221,
	    				"data":"BASE64 image data"
	    				}
	    			"left_code":
	    				{
	    				"length":123221,
	    				"data":"BASE64 code data"
	    				}
	    			"right_image":
	    				{
	    				"width":520,
	    				"height":690,
	    				"format":2,
	    				"length":123221,
	    				"data":"BASE64 image data"
	    				}
	    			"right_code":
	    				{
	    				"length":123221,
	    				"data":"BASE64 code data"
	    				}
	    		 	}
             }

* 返回JSON示例

             {
                 "mod":"download_person",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":2,
                 "code":0,
                 "message":""
             }
         
### 上传考勤记录
* 请求顺序
	客户端 -> 服务器

* 请求JSON示例
  
             {
                 "mod":"upload_access_info",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":1,
                 "access_info":
	    			{
	    			"person_id":"3413123123"
	    			"time":"20190823123435",
	    			"id_number":“1545123",
	    			"card_number":"123123123",
	    			"role":12,
	    			"open_door_type":2,
	    			"in_out_type":2,
	    			"community_code":"1231321"
	    		 	}
             }

* 返回JSON示例

             {
                 "mod":"upload_access_info",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":2,
                 "code":0,
                 "message":""
             }
             
### 服务器自定义命令
* 请求顺序
	服务器 -> 客户端

* 请求JSON示例
  
             {
                 "mod":"send_user_command",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":1,
                 "command_type":2
                 "command_value":"test!"
             }

* 返回JSON示例

             {
                 "mod":"send_user_command",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":2,
                 "code":0,
                 "message":""
             }
             
### 客户端自定义命令
* 请求顺序
	服务器 -> 客户端

* 请求JSON示例
  
             {
                 "mod":"send_device_command",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":1,
                 "command_type":2
                 "command_value":"test!"
             }

* 返回JSON示例

             {
                 "mod":"send_device_command",
                 "session_id":"de6a6236-db52-11e9-aaa8-00e04c110030",
                 "type":2,
                 "code":0,
                 "message":""
             }