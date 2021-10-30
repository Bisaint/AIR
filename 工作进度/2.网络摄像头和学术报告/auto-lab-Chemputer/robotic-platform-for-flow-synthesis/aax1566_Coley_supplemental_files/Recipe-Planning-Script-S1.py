import pandas as pd 
import numpy as np
import argparse
import re
from math import nan

'''This script takes a process config spreadsheet and generates a recipe file with
a lot of placeholder values. It expects a pretty particular format for the table'''


THIN_TUBING_REAGENT_INT = [3, 5, 6] # smaller syringes == prime @ 4 uL/s instead of 5

def isnan(x):
    try:
        return np.isnan(x)
    except TypeError:
        return False

def main(fpath, name, rline, recipe_type):
    xlsx = pd.ExcelFile(fpath)
    sheet = xlsx.parse(xlsx.sheet_names[0]).values

    # Offset to anchor
    x0, y0 = np.where(sheet == 'ANCHOR')
    x0, y0 = x0[0], y0[0]
    sheet = sheet[x0:, y0:]

    # Figure out where operator instructions defined
    x1, y1 = np.where(sheet == 'Reactor storage instructions')
    x1, y1 = x1[0], y1[0]

    col = 0
    for col in range(sheet.shape[1]):
        try:
            if sheet[0, col].lower() == name:
                break
        except:
            pass
    else:
        raise ValueError('Target not found!')
    print('Relevant column = {}'.format(col))
    settings = sheet[:, col:col+2]

    # Special storage instructions / plumbing
    for i in range(0, 14):
        if isnan(sheet[x1+i,col]) or sheet[x1+i, col] == 'n/a':
            continue
        rline('### {}'.format(sheet[x1+i, y1]))
        rline('# {}'.format(sheet[x1+i, col]))
    rline()

    pressure = int(float(settings[0,1]))

    INCLUDE_TUBE_COMMANDS = recipe_type != 'bays'
    INCLUDE_BAY_COMMANDS = recipe_type != 'tubes'
    CRASHED_RECIPE = recipe_type == 'guicrashed'
    INCLUDE_CHEM_COMMANDS = recipe_type == 'full' or CRASHED_RECIPE


    # Define modules (top to bottom)
    reactors = []
    ports = []
    for bay_num in range(5):
        row_start = 1 + bay_num * 5
        if isnan(settings[row_start, 0]):
            continue

        # What reactor unit is supposed to go here?
        title = settings[row_start, 0]
        reactor_id = int(re.findall('ID ([0-9]*)', title)[0])
        temperature = None 
        try:
            temperature = int(float(settings[row_start, 1]))
        except:
            pass
        reactors.append({
            'bay_num': bay_num, 
            'reactor_id': reactor_id,
            'title': title,
            'temperature': temperature,
        })

        # What port connections need to be made?
        port_list = [3,2,1,4] if bay_num == 4 else [4,3,2,1]
        for i, port_id in enumerate(port_list):
            if isnan(settings[row_start+i+1, 0]):
                continue
            flow_sp = None
            try:
                flow_sp = float(settings[row_start+i+1,1])
            except Exception as e:
                print('Could not parse flow setpoint')
                print(e)
                continue
            ports.append({
                'bay_num': bay_num,
                'reactor_id': reactor_id,
                'port_id': port_id,
                'reagent_int': int(settings[row_start+i+1, 0].split(',')[-1]),
                'flow_sp': flow_sp,
                'flow_sp_uls': flow_sp / 60.0,
                'reagent': settings[row_start+i+1, 0],
            })

    rline('# Define reactor positions')
    for reactor in sorted(reactors, key=lambda x: x['bay_num']):
        rline('REACTORSETUP,{reactor_id:d},0 {bay_num:d}'.format(**reactor))
    rline()


    rline('# Define pump connections')
    for port in sorted(ports, key=lambda x: x['reagent_int']):
        rline('PUMPSETUP,{reactor_id:d},{port_id:d},0 {reagent_int:d}'.format(**port))
    rline()
    

    if INCLUDE_TUBE_COMMANDS and not CRASHED_RECIPE:
        rline('# Make connections')
        for port in sorted(ports, key=lambda x: x['reagent_int']):
            rline('PUMPCONNECT,{reactor_id:d},{port_id:d}'.format(**port))
        rline()

    if INCLUDE_BAY_COMMANDS and not CRASHED_RECIPE:
        rline('# Move reactors into place')
        for reactor in sorted(reactors, key=lambda x: x['bay_num']):
            rline('REACTORMOVE,{reactor_id:d},TOBAY'.format(**reactor))
        rline()


    if INCLUDE_CHEM_COMMANDS and not CRASHED_RECIPE:
        rline('# Give 60 seconds to check before sealing')
        rline('TIMEOUT,60000')
        rline()

        rline('# Close jaws to seal path')
        for reactor in sorted(reactors, key=lambda x: x['bay_num']):
            rline('REACTORJAWCLOSE,{reactor_id:d}'.format(**reactor))
        rline('STACKCLOSE,0')
        rline('REAGENTTREECLOSE,0')
        rline()



        rline('# Open on-board valves')
        for reactor in sorted(reactors, key=lambda x: x['bay_num']):
            if int(reactor['reactor_id']) == 6:
                continue # outlet dosen't need actuation
            rline('PORTOPEN,{reactor_id:d},inlet'.format(**reactor))
            rline('PORTOPEN,{reactor_id:d},outlet'.format(**reactor))
        for port in sorted(ports, key=lambda x: x['reagent_int']):
            if int(port['reactor_id']) == 6:
                continue # outlet dosen't need actuation
            if int(port['bay_num'] == 4) and port['port_id'] == 4:
                continue # this is actually the inlet
            rline('PORTOPEN,{reactor_id:d},{port_id:d}'.format(**port))
        rline()


        rline('# Start bluetooth')
        rline('TIMEOUT,5000')
        for reactor in sorted(reactors, key=lambda x: x['bay_num']):
            if reactor['temperature']:
                rline('BLECONNECT,{reactor_id:d}'.format(**reactor))
                rline('TIMEOUT,10000')
        rline()


        rline('# Set relevant valves to wash')
        for port in sorted(ports, key=lambda x: x['reagent_int']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            rline('WASHVALVESON,{reactor_id:d},{port_id:d}'.format(**port))
        rline()


        # Start rinsing
        rline('### Start rinse flow @ 10 uL/s (TODO: when using the packed bed, this is bad)')
        rline('RINSESET,0,10')
        rline()

        rline('# Start wash priming flow @ 5 uL/s (TODO: when using packed bed, this is bad)')
        for port in sorted(ports, key=lambda x: x['reagent_int']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            if port['reagent_int'] not in THIN_TUBING_REAGENT_INT:
                rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},5'.format(**port))
            else:
                rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},4'.format(**port))
        rline()


        rline('# Prime with solvent for 15 minutes (ambient temp, pres)')
        rline('TIMEOUT,900000')
        rline()



        rline('# Stop priming flow')
        for port in sorted(ports, key=lambda x: x['reagent_int']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},0'.format(**port))
        rline()



        rline('# Set reagent selector valves (TODO: check this! This is not automatic)')
        rline('REAGENTPORT,1,1')
        rline('REAGENTPORT,2,1')
        rline()


        rline('### Start all reagent lines at 5 uL/s for 4 minutes - get air out of lines')
        for port in sorted(ports, key=lambda x: x['bay_num']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            rline('WASHVALVESOFF,{reactor_id:d},{port_id:d}'.format(**port))
            if port['reagent_int'] not in THIN_TUBING_REAGENT_INT:
                rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},5'.format(**port))
            else:
                rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},4'.format(**port))
        rline('TIMEOUT,240000')
        for port in sorted(ports, key=lambda x: x['bay_num']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},0'.format(**port))
        rline('TIMEOUT,5000')
        rline()



        rline('# Set back pressure for system')
        rline('PRESSURESET,0,{:d}'.format(pressure))
        rline()


        rline('# Set temperatures (step to 1/2)')
        for reactor in sorted(reactors, key=lambda x: x['bay_num']):
            if reactor['temperature'] is None or isnan(reactor['temperature']):
                continue
            rline('SETPOINT,{reactor_id:d},{halftemp:d}'.format(**reactor, halftemp=25+int((reactor['temperature']-25)/2)))
        rline('TIMEOUT,60000')
        rline('# Set temperatures (step 2/2)')
        for reactor in sorted(reactors, key=lambda x: x['bay_num']):
            if reactor['temperature'] is None or isnan(reactor['temperature']):
                continue
            rline('SETPOINT,{reactor_id:d},{temperature:d}'.format(**reactor))
        if not CRASHED_RECIPE:
            rline('TIMEOUT,60000')
        rline()


    if INCLUDE_CHEM_COMMANDS and not CRASHED_RECIPE:
        rline('### Prime lines one-by-one from downstream to upstream for 8 more minutes (TODO: confirm times)')
        for port in sorted(ports, key=lambda x: x['bay_num']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            rline('# Wait 6 min @ 5 uL/s for line {} to prime'.format(port['reagent']))
            if port['reagent_int'] not in THIN_TUBING_REAGENT_INT:
                rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},5'.format(**port))
            else:
                rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},4'.format(**port))
            rline('TIMEOUT,360000')
            rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},0'.format(**port))
            rline('TIMEOUT,30000')
        rline()


    if INCLUDE_CHEM_COMMANDS:

        rline('# Start real flows')
        for port in sorted(ports, key=lambda x: x['reagent_int']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},{flow_sp_uls:.4f}'.format(**port))
        rline()

        rline('### Stop rinse flow')
        rline('RINSESET,0,0')
        rline()


        # Restart bluetooth if necessary -- AFTER setting real flow rates
        if CRASHED_RECIPE:
            rline('# Start bluetooth')
            rline('TIMEOUT,5000')
            for reactor in sorted(reactors, key=lambda x: x['bay_num']):
                if reactor['temperature']:
                    rline('BLECONNECT,{reactor_id:d}'.format(**reactor))
                    rline('TIMEOUT,10000')
                if reactor['temperature'] is None or isnan(reactor['temperature']):
                    continue
                rline('SETPOINT,{reactor_id:d},{temperature:d}'.format(**reactor))
            rline()


        rline('################################################')
        rline('# Hold for chemistry')
        rline('PAUSE')
        rline('################################################')
        rline()


        # Stop all pumps
        rline('# Stop all pumps')
        for port in sorted(ports, key=lambda x: x['reagent_int']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},0'.format(**port))
        rline()

        # Start blasting rinse for 15 minutes
        rline('### Blast rinse for 15 minutes')
        rline('RINSESET,0,10')
        rline('TIMEOUT,900000')
        rline()

        rline('### Blast the reagent lines out one-by-one from top to bottom (TODO: Check order, PBR flow rates)')
        for port in sorted(ports, key=lambda x: x['bay_num']*100+x['reagent_int']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            rline('WASHVALVESON,{reactor_id:d},{port_id:d}'.format(**port))
            if port['reagent_int'] not in THIN_TUBING_REAGENT_INT:
                rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},5'.format(**port))
            else:
                rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},4'.format(**port))
            rline('TIMEOUT,300000')
            rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},0'.format(**port)) 
        rline()

        rline('### Blast the reagent lines out all together (PBR flow rates)')
        for port in sorted(ports, key=lambda x: x['bay_num']*100+x['reagent_int']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},5'.format(**port))   
        rline()


        rline('# Unset temperatures')
        for reactor in sorted(reactors, key=lambda x: x['bay_num']):
            if reactor['temperature'] is None or isnan(reactor['temperature']):
                continue
            rline('SETPOINT,{reactor_id:d},0'.format(**reactor))
        rline('TIMEOUT,300000')
        rline()


        rline('# Unset back pressure')
        rline('PRESSURESET,0,0')
        rline()


        rline('# Stop bluetooth')
        for reactor in sorted(reactors, key=lambda x: x['bay_num']):
            if reactor['temperature']:
                rline('BLEDISCONNECT,{reactor_id:d}'.format(**reactor))
                rline('TIMEOUT,1000')
        rline()


        rline('### Stop all flow after 10 additional rinse minutes')
        rline('TIMEOUT,900000')
        for port in sorted(ports, key=lambda x: x['bay_num']*100+x['reagent_int']):
            if port['flow_sp'] is None or isnan(port['flow_sp']):
                continue
            rline('PUMPFLOWSET,{reactor_id:d},{port_id:d},0'.format(**port)) 
        rline('RINSESET,0,0')
        rline()


        rline('# Close on-board valves')
        for reactor in sorted(reactors, key=lambda x: x['bay_num']):
            if int(reactor['reactor_id']) == 6:
                continue # outlet dosen't need actuation
            rline('PORTCLOSE,{reactor_id:d},inlet'.format(**reactor))
            rline('PORTCLOSE,{reactor_id:d},outlet'.format(**reactor))
        for port in sorted(ports, key=lambda x: x['reagent_int']):
            if int(port['reactor_id']) == 6:
                continue # outlet dosen't need actuation
            if int(port['bay_num'] == 4) and port['port_id'] == 4:
                continue # this is actually the inlet
            rline('PORTCLOSE,{reactor_id:d},{port_id:d}'.format(**port))
        rline('TIMEOUT,5000')
        rline()


        # Deconstruct
        rline('# Open all connections')
        rline('REAGENTTREEOPEN,0')
        rline('TIMEOUT,2000')
        rline('STACKOPEN,0')
        rline('TIMEOUT,2000')
        for reactor in sorted(reactors, key=lambda x: x['bay_num'], reverse=True):
            rline('REACTORJAWOPEN,{reactor_id:d}'.format(**reactor))
            rline('TIMEOUT,1000')
        rline()


    if INCLUDE_BAY_COMMANDS:
        rline('# Move reactors into storage')
        for reactor in sorted(reactors, key=lambda x: x['bay_num'], reverse=True):
            rline('REACTORMOVE,{reactor_id:d},TOSTORAGE'.format(**reactor))
        rline()


    if INCLUDE_TUBE_COMMANDS:
        rline('# Disconnect lines')
        for port in sorted(ports, key=lambda x: x['reagent_int'], reverse=True):
            rline('PUMPDISCONNECT,{reactor_id:d},{port_id:d}'.format(**port))
        rline()



if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert xlsx to recipe file')
    parser.add_argument('--file', dest='file', help='Where is the xlsx located?',
        default='/Users/ccoley/Dropbox (MIT)/APECS Manuscript/SI - Other files/File S1 - Process configurations.xlsx')
    args = parser.parse_args()

    names = ['aspirin', 'secnidazole', 'lidocaine', 'diazepam', '(s)-warfarin', 'safinamide', 
        'ace inhibitor library (long)', 'ace inhibitor library (short)', 'nsaid library']
    recipe_types = ['full', 'bays', 'tubes', 'guicrashed']

    for name in names:
        for recipe_type in recipe_types:
            with open('recipe_{}_{}.csv'.format(recipe_type, name), 'w') as fid:
                def rline(txt=''):
                    if txt and txt[0] != '#':
                        fid.write('{:03d},{}\n'.format(rline.i, txt))
                        rline.i += 1
                    elif txt:
                        fid.write('{}\n'.format(txt))
                    else:
                        fid.write('\n\n')
                rline.i = 1
                fid.write('# Recipe file (outline) for {}\n'.format(name))
                rline()
                main(args.file, name, rline, recipe_type)

