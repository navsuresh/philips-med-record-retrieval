from flask import Flask, render_template, request, flash  
from forms import QueryForm  
app = Flask(__name__)  
app.secret_key = 'development key'  

from flask_table import Table, Col
 
class Results(Table):
   score = Col('Score')
   title = Col('Title')
 
@app.route('/', methods = ['GET', 'POST'])  
def contact():  
   form = QueryForm()  
   if form.validate() == False:  
      flash('All fields are required.')  
   return render_template('contact.html', form = form, table = "")  
 
 
 
@app.route('/success',methods = ['GET','POST'])  
def success():  
   form = QueryForm()  
   if form.validate() == False:  
      flash('All fields are required.')  
   req = request.form['query']
   print("REQUEST: ", req)
   results = search(req)
   table = Results(results)
   table.border = True
   return render_template("contact.html", table = table, form = form) 

def search(req):
   return [dict(score='0.98', title=req),
         dict(score='0.91', title='Medical Record 2'),
         dict(score='0.84', title='Medical Record 3')] 
  
if __name__ == '__main__':  
   app.run(debug = True)  