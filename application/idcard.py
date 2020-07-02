#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Aug  4 01:01:37 2019
身份证识别
@author: chineseocr
"""
from apphelper.image import union_rbox
import re


class idcard:
    """
    身份证结构化识别
    """
    
    def __init__(self, result):
        self.result = union_rbox(result, 0.2)
        self.N = len(self.result)
        self.res = {}
        self.full_name()
        self.sex()
        self.birthday()
        self.birthNo()
        self.address()
    
    def full_name(self):
        """
        身份证姓名
        """
        name = {}
        for i in range(self.N):
            txt = self.result[i]['text'].replace(' ', '')
            txt = txt.replace(' ', '')
            # 匹配身份证姓名
            res = re.findall("姓名[\u4e00-\u9fa5]{1,4}", txt)
            if len(res) > 0:
                name['姓名'] = res[0].replace('姓名', '')
                self.res.update(name)
                break
    
    def sex(self):
        """
        性别女民族汉
        """
        sex = {}
        for i in range(self.N):
            txt = self.result[i]['text'].replace(' ', '')
            txt = txt.replace(' ', '')
            if '男' in txt:
                sex["性别"] = '男'
            elif '女' in txt:
                sex["性别"] = '女'
            
            ##性别女民族汉
            res = re.findall(".*民族[\u4e00-\u9fa5]+", txt)
            if len(res) > 0:
                sex["民族"] = res[0].split('民族')[-1]
                self.res.update(sex)
                break
    
    def birthday(self):
        """
        出生年月
        """
        birth = {}
        for i in range(self.N):
            txt = self.result[i]['text'].replace(' ', '')
            txt = txt.replace(' ', '')
            # 出生年月
            res = re.findall('出生\d*年\d*月\d*日', txt)
            res = re.findall('\d*年\d*月\d*日', txt)
            
            if len(res) > 0:
                birth['出生年月'] = res[0].replace('出生', '').replace('年', '-').replace('月', '-').replace('日', '')
                self.res.update(birth)
                break
    
    def birthNo(self):
        """
        身份证号码
        """
        No = {}
        for i in range(self.N):
            txt = self.result[i]['text'].replace(' ', '')
            txt = txt.replace(' ', '')
            # 身份证号码
            res = re.findall('号码\d*[X|x]', txt)
            res += re.findall('号码\d*', txt)
            res += re.findall('\d{16,18}', txt)
            
            if len(res) > 0:
                No['身份证号码'] = res[0].replace('号码', '')
                self.res.update(No)
                break
    
    def address(self):
        """
        身份证地址
        ##此处地址匹配还需完善
        """
        add = {}
        addString = []
        for i in range(self.N):
            txt = self.result[i]['text'].replace(' ', '')
            txt = txt.replace(' ', '')
            
            # 身份证地址
            if '住址' in txt or '省' in txt or '市' in txt or '县' in txt or '街' in txt or '村' in txt or "镇" in txt or "区" in txt or "城" in txt:
                addString.append(txt.replace('住址', ''))
        
        if len(addString) > 0:
            add['身份证地址'] = ''.join(addString)
            self.res.update(add)
