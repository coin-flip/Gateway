__author__ = 'shams'
import json
import types
import copy
class action:
    connect = "connectToCloud"
    getServices = "getServicesForUser"
    execService = "executeService"
class retType:
    b64="b64img"
    str = "string"
    list = "list"

class jFlip:
    @staticmethod
    def receive(rec):
        return json.loads(str(rec))
    @staticmethod
    def connectToCloud(gatewayID,users,gatewayName):
        assert type(users) is types.ListType
        connect= {
             "gatewayName":gatewayName,
             "action": action.connect,
             "gatewayID":"",
             "users":[]
        }
        connect["users"] = users[:]
        connect["gatewayID"] = gatewayID
        return json.dumps(connect)

    @staticmethod
    def makeService(name,paramCnt,params,ret):
        """ returns a service as a list ready to be passed to sendServices
        """
        assert type(params) is types.ListType
        service = {"serviceName": "",
         "serviceParameterCount": "",
         "serviceParameters": [],
         "serviceReturnType": "", #string/b64img/list
         }
        service["serviceName"] = name
        service["serviceParameterCount"] = str(paramCnt)
        service["serviceParameters"] = params[:]
        service["serviceReturnType"] = ret
        return copy.deepcopy(service)

    @staticmethod
    def getServicesSuccess(services,userID):
        sendServicesSuccess = {
            "action": action.getServices,
            "returnCode":"SUCCESS",
            "serviceCount":int(len(services)),
            "services":[],
            "userID":userID
        }
        sendServicesSuccess["services"] = services
        return json.dumps(sendServicesSuccess)

    @staticmethod
    def getServicesFailure(reason,userID):
        getServicesFailure = {
            "action": action.getServices,
            "returnCode": "FAILED",
            "failReason": "", #NoServices/NotAuthenticated/NotAllowedbyAccessControl
            "userID":userID
        }
        getServicesFailure["failReason"] = str(reason)
        return json.dumps(getServicesFailure)

    @staticmethod
    def execServiceSuccess(userID,serviceName,serviceReturnType,ret):
        assert type(ret) is types.StringType
        executeServiceSuccess = {
            "userID":userID,
            "returnCode":"SUCCESS",
            "serviceName":"reverseString",
            "serviceReturnType":"", #string/b64img/list
             "returnPayload":"" #return stuff/or success
        }
        executeServiceSuccess["userID"] = str(userID)
        executeServiceSuccess["serviceName"] = str(serviceName)
        executeServiceSuccess["serviceReturnType"] = str(serviceReturnType)
        executeServiceSuccess['returnPayload'] = str(ret)
        return json.dumps(executeServiceSuccess)

    @staticmethod
    def execServiceFailure(failReason):
        executeServiceFailure = {
            "action": action.execService,
            "returnCode": "FAILED",
            "failReason": "" #ServiceNotUpdated/ServiceNotUp/ServiceBusy..etc
        }
        executeServiceFailure["failReason"] = str(failReason)
        return json.dumps(executeServiceFailure)


