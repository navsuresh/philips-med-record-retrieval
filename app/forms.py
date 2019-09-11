from flask_wtf import Form
from wtforms import TextField, IntegerField, TextAreaField, SubmitField, RadioField,SelectField

from wtforms import validators, ValidationError

class ContactForm(Form):

   
   email = TextField("",[validators.Required("Enter Search Query")])
   

   submit = SubmitField("Search")