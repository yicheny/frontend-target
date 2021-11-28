const mysql      = require('mysql');
const { clearTimeout } = require('timers');

const connection = mysql.createConnection({
  host     : '127.0.0.1',
  user     : 'root',
  password : 'abc123',
  database : 'sqltest',
});
 
connection.connect((error)=>{
    if(error) return console.log('连接失败',error.message)
    console.log('连接成功')
});
 
connection.query('SELECT * FROM student', function (error, results, fields) {
  if (error) throw error;
  console.log('当前查询结果', results);
});

const timeId = setTimeout(()=>{
    connection.destroy()
    console.log('关闭连接')
    clearTimeout(timeId)
},5000)