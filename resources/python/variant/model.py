class ModelBase:
    def __init__(self):
        self._parameters = dict()
        self._variables = dict()
        self._solved = False

    @property
    def parameters(self):
        """Input parameters"""
        return self._parameters

    @parameters.setter
    def parameters(self, values):
        self._parameters = values

    @property
    def variables(self):
        """Output variables"""
        return self._variables

    @variables.setter
    def variables(self, values):
        self._variables = values

    @property
    def solved(self):
        """Problem is solved"""
        return self._solved

    @solved.setter
    def solved(self, solv):
        self._solved = solv        
        
    def create(self):
        pass

    def solve(self):
        pass

    def process(self):
        pass           
        
    def load(self, filename):
        import xml.etree.ElementTree as ET
        
        tree = ET.parse(filename)
        variant = tree.getroot()  
                 
        results = variant.findall('results')[0]
        result = results.findall('result')[0]

        solution = result.findall('solution')[0]
        solved = int(solution.attrib['solved'])
            
        # input
        input = result.findall('input')[0]            
        for par in input.findall('parameter'):
            self.parameters[par.attrib["name"]] = float(par.attrib["value"])
                
        # output
        output = result.findall('output')[0]
        if (solved):
            for var in output.findall('variable'):
                try:
                    self.variables[var.attrib["name"]] = float(var.attrib["value"])
                except ValueError:
                    self.variables[var.attrib["name"]] = var.attrib["value"]            


    def save(self, filename):
        import xml.etree.cElementTree as ET
        
        variant = ET.Element("variant:variant")        
        variant.set("xmlns:variant", "XMLOptVariant")
        variant.set("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")

        # results    
        results = ET.SubElement(variant, "results")                               
        result = ET.SubElement(results, "result")   
                                                                            
        # input
        input = ET.SubElement(result, "input")
        for key, value in self.parameters.items():
            parameter = ET.SubElement(input, "parameter")
            parameter.set("name", key)
            parameter.set("value", str(value))
        
        # output
        output = ET.SubElement(result, "output")
        if (self.solved):
            for key, value in self.variables.items():
                variable = ET.SubElement(output, "variable")
                variable.set("name", key)
                variable.set("value", str(value))        
            
        # solution
        solution = ET.SubElement(result, "solution")
        solution.set("solved", "1" if self.solved else "0")                      
                        
        tree = ET.ElementTree(variant)
        tree.write(filename, xml_declaration = True, encoding='UTF-8')        
        
class ModelDict:
    def __init__(self):
        self._models = []
        self._parameters = dict()
        self._filename = ""
                                
    @property
    def models(self):
        return self._models

    @models.setter
    def models(self, values):
        self._models = values
        
    @property
    def parameters(self):
        """Input parameters"""
        return self._parameters

    @parameters.setter
    def parameters(self, values):
        self._parameters = values
                
    def load(self, filename):
        import xml.etree.ElementTree as ET
        
        tree = ET.parse(filename)
        variant = tree.getroot()  
        
        problem = variant.findall('problem')[0]
        for par in problem.findall('input_param'):
            self.parameters[par.attrib["name"]] = par.attrib["unit"]
         
        results = variant.findall('results')[0]
        for result in results.findall('result'):
            model = ModelBase()

            solution = result.findall('solution')[0]
            model.solved = int(solution.attrib['solved'])
            
            # input
            input = result.findall('input')[0]            
            for par in input.findall('parameter'):
                model.parameters[par.attrib["name"]] = float(par.attrib["value"])
                
            # output
            output = result.findall('output')[0]
            if (model.solved):
                for var in output.findall('variable'):
                    model.variables[var.attrib["name"]] = var.attrib["value"]
                
            self.models.append(model)

    def save(self, filename):
        import xml.etree.cElementTree as ET
        
        variant = ET.Element("variant:variant")        
        variant.set("xmlns:variant", "XMLOptVariant")
        variant.set("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")

        # problem
        problem = ET.SubElement(variant, "problem")                           
        for key, value in self.parameters.items():                           
            input_parameter = ET.SubElement(problem, "input_parameter")
            input_parameter.set("param", key)
            input_parameter.set("unit", value)
                                                   
        # results    
        results = ET.SubElement(variant, "results")                               
        for model in self.models:
            result = ET.SubElement(results, "result")   
                                                                            
            # input
            input = ET.SubElement(result, "input")
            for key, value in model.parameters.items():
                parameter = ET.SubElement(input, "parameter")
                parameter.set("name", key)
                parameter.set("value", str(value))
            
            # output
            output = ET.SubElement(result, "output")
            if (model.solved):
                for key, value in model.variables.items():
                    variable = ET.SubElement(output, "variable")
                    variable.set("name", key)
                    variable.set("value", str(value))        
                
            # solution
            solution = ET.SubElement(result, "solution")
            solution.set("solved", "1" if model.solved else "0")                      
                        
        tree = ET.ElementTree(variant)
        tree.write(filename, xml_declaration = True, encoding='UTF-8')
        
     