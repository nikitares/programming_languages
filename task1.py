import sys
from xml.dom.minidom import parse
import csv
import resistance_calc

def parse_resistors(root_node):
    resistors = []
    for resistor in root_node.getElementsByTagName("resistor"):
        resistors.append( [int(resistor.attributes["net_from"].value), int(resistor.attributes["net_to"].value), 
                           float(resistor.attributes["resistance"].value), float(resistor.attributes["resistance"].value)] )
    return resistors

def parse_capactors(root_node):
    capactors = []
    for capactor in root_node.getElementsByTagName("capactor"):
        capactors.append( [int(capactor.attributes["net_from"].value), int(capactor.attributes["net_to"].value), 
                           float(capactor.attributes["resistance"].value), float(capactor.attributes["resistance"].value)] )
    return capactors

def parse_diodes(root_node):
    diodes = []
    for diode in root_node.getElementsByTagName("diode"):
        diodes.append( [int(diode.attributes["net_from"].value), int(diode.attributes["net_to"].value), 
                        float(diode.attributes["resistance"].value), float(diode.attributes["reverse_resistance"].value)] )
    return diodes

def read_xml_file(filepath) -> list:
    dom = parse(filepath)
    dom = dom.getElementsByTagName("schematics")[0]
    return parse_resistors(dom) + parse_capactors(dom) + parse_diodes(dom)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Error: Usage task1 <input filepath> <outpur filepath>")
        sys.exit(1)
    circuit = read_xml_file(sys.argv[1])
    net = resistance_calc.faster_calculate(circuit)
    with open(sys.argv[2], 'w') as file:
        csv_writer = csv.writer(file, delimiter=',', lineterminator=',\n')
        csv_writer.writerows(net)
