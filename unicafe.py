import time, socket, urllib2, datetime

def getfoods(campus, day, restaurant):
    if (restaurant==0): restaurant=10
    elif (restaurant==1): restaurant=11
    day = day + 1
    parsedfoods=[]

    if (day>5):
        parsedfoods.append("Weekend.")
        return parsedfoods
    try:
        url = "http://www.unicafe.fi/lounas/?kampus=" + str(campus) + "&ravintola=" + str(restaurant) + "&paiva=" + str(day)
        unicafe = urllib2.urlopen(url)
        source = unicafe.read()
        foods = source.split('<ul><li>')[1].split('</div>')[0]
        foods = foods.split('</li><li>')

        for food in foods:
            food = food.split('<span')
            type = food.pop()
            food = food[0].split(' (')[0]
            type = type.split('<span class="price"')[0].split('>')[1].split('<')[0].split(' ')[0]
            parsedfoods.append(food + " - " + type)

        return parsedfoods
    except:
        parsedfoods.append("Either no food available or an error occured.")
        return parsedfoods

def unicafe():
	day = time.localtime(time.time())[6]
	hour = time.localtime(time.time())[3]
	over17=0
	if (hour>17):
		day=day+1
		over17=1
	dt=datetime.datetime.now()
	curday=dt.day+over17
	if(curday==32): curday=1
	ret="Food for: " + str(curday) + "." + str(dt.month) + "." + str(dt.year) + "\n*----*\n"
	ret+="Chemicum:\n"
	try:
		parsedfoods = getfoods(3,day,0)
		for food in parsedfoods:
			ret+=food
			ret+="\n"
	except:
		ret+="Either no food available or an error occured."
		ret+="\n"
	ret+="*----*\n"
	ret+="Exactum:\n"
	try:
		parsedfoods = getfoods(3,day,1)
		for food in parsedfoods:
			ret+=food
			ret+="\n"
	except:
		ret+="Either no food available or an error occured."
		ret+="\n"

	return ret

def unicafe_centre():
	day = time.localtime(time.time())[6]
	hour = time.localtime(time.time())[3]
	over17=0
	if (hour>17):
		day=day+1
		over17=1
	dt=datetime.datetime.now()
	curday=dt.day+over17
	if(curday==32): curday=1
	ret="Food for: " + str(curday) + "." + str(dt.month) + "." + str(dt.year) + "\n*----*\n"
	ret+="Ylioppilasaukio:\n"
	try:
		parsedfoods = getfoods(1,day,8)
		for food in parsedfoods:
			ret+=food
			ret+="\n"
	except:
		ret+="Either no food available or an error occured."
		ret+="\n"
	ret+="*----*\n"
	ret+="Porthania:\n"
	try:
		parsedfoods = getfoods(1,day,3)
		for food in parsedfoods:
			ret+=food
			ret+="\n"
	except:
		ret+="Either no food available or an error occured."
		ret+="\n"

	return ret
