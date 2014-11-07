__author__ = 'shams'
import ConfigParser
import subprocess
import types
import re

class services:
    def __init__(self,iniFilename ):
        self.filename = iniFilename

    def sectionToMap(self, section):
        dict1 = {}
        config = ConfigParser.ConfigParser()
        config.read(self.filename)
        options = config.options(section)
        for option in options:
                dict1[option] = config.get(section, option)
        return dict1

    def getServices(self):
        config = ConfigParser.ConfigParser()
        config.read(self.filename)
        ret = []
        for section in  config.sections():
            service = {}
            service["serviceName"] = str(section)
            dict = self.sectionToMap(section)
            paramNames=[]
            paramTypes=[]
            for option in dict:
                if str(option) == "parametercount":
                    service["parameterCount"] = dict[option]
                elif re.search(r'parameter_[0-9]_name',str(option))!=None:
                    paramNames.append(dict[option])
                elif re.search(r'parameter_[0-9]_type',str(option))!=None:
                    paramTypes.append(dict[option])
                elif str(option) == "type":
                    service["type"] = dict[option]
                elif str(option) == "language":
                    service["language"] = dict[option]
                elif str(option) == "filename":
                    service["filename"] = dict[option]
                elif str(option) == "return_type":
                    service["returnType"]=dict[option]
                else:
                    raise ValueError(option)
            assert(len(paramNames) == len(paramTypes)==int(dict["parametercount"]))
            service["parameters"]=[]
            for i in range(len(paramNames)):
                service["parameters"].append({"parameterName": str(paramNames[i]), "parameterType": str(paramTypes[i])})
            ret.append(service)
        return ret

    def executeService(self,service):
        """ service has the actual parameters instead of the formal parameters
        """
        assert type(service) == types.DictionaryType
        command=[]
        if service["language"]=="python":
            command.append("python")
            command.append(service["filename"])
            for param in service["parameters"]:
                command.append(param)
            process = subprocess.Popen(command, stdout=subprocess.PIPE)
            out, err = process.communicate()
            return out
        elif service["language"] == "c" or service["language"] == "c++":
            proc = subprocess.Popen(["ls",str(service["serviceName"])+".o"],stdout=subprocess.PIPE)
            out,err = proc.communicate()
            if(out == str(service["serviceName"])+".o\n"):
                args = []
                args.append("./"+str(service["serviceName"])+".o")
                for arg in service["parameters"]:
                    args.append(arg)
                process = subprocess.Popen(args,stdout=subprocess.PIPE)
                out,err = process.communicate()
                return out
            else:
                process = subprocess.Popen(["g++",str(service["filename"]),"-o",str(service["serviceName"])+".o"],stdout=subprocess.PIPE)
                args = []
                args.append("./"+str(service["serviceName"])+".o")
                for arg in service["parameters"]:
                    args.append(arg)
                process2 = subprocess.Popen(args,stdout=subprocess.PIPE)
                out,err = process2.communicate()
                return out




