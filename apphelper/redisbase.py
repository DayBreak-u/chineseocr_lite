#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Aug  4 23:30:48 2019

@author: chineseocr
"""
import redis
from apphelper.image import PIL_to_base64,base64_to_PIL
pool = redis.ConnectionPool(host='localhost', port=6379, decode_responses=True)   # host是redis主机，需要redis服务端和客户端都起着 redis默认端口是6379
conn = redis.Redis(connection_pool=pool)
jobListTable = 'job_list_table'##job列表
resSetTable = 'res_set_table'##识别结果列表
imgStringTable='img_string_table'##图像存储表
import time
import uuid

class redisDataBase:
    """
    redis相关操作
    """
    def put_values(self,resJob,timeOut=20):
        ##向队列推送PIL 并返回识别结果
        timeBegin = time.time()
        listName = uuid.uuid1().__str__()
        keys = []
        for ind,line in enumerate(resJob):
            img = line['img']
            value = PIL_to_base64(img)
            ind = '{}_{}'.format(listName,ind)
            keys.append(ind)
            self.set_dict(imgStringTable,ind,value)
            self.set_list(jobListTable,ind)
            
            
        res = [None for key in keys]
        while time.time()-timeBegin<timeOut:
            res = self.get_dict(resSetTable,keys)
            delres = [x for x in res if x is not None]
            if len(delres)==len(keys):
                break
            
        ##删除记录
        for key in keys:
            self.del_dict(imgStringTable,key)##删除缓存图像
            self.del_dict(resSetTable,key)##删除缓存图像
        
                
        for ind,text in enumerate(res):
            resJob[ind]['text'] = text
            
        return resJob
              
        
             
        
    def get_job(self,callback):
        ##获取队列中的图像
        ind = self.get_list(jobListTable)
        
        if ind is not None:
            value = self.get_dict(imgStringTable,[ind])
            value = value[0]
            if value is not None:
                value     =  base64_to_PIL(value)
                if value is not None:
                    value = value.convert('L')
                    value     =  callback(value)
                else:
                    value=''
                print(ind,value)
                self.set_dict(resSetTable,ind,value)##存储识别结果到set
                
                        


    def set_list(self,name,value):
        """
        向队列中推送识别值
        """
        conn.rpush(name,value)
        
    def get_list(self,name):
        """
        获取队列值
        """
        value = conn.lpop(name)
        return value
    
    def set_dict(self,name,key,value):
        """
        设置键值对
        """
        conn.hset(name, key,value)
        
    def get_dict(self,name,keys):
        """
        批量取出
        """
        return conn.hmget(name, keys)
    
    
    def del_dict(self,name,key):
        conn.hdel(name, key)
        
        
    

    
    
    
    
 
