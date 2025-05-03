const express = require('express');   // Import express{(express) to create a server}
const mongoose = require('mongoose'); // Import mongoose{(mongoose) to connect to MongoDB}
const cors = require('cors');         //import cors{(cors) to enable CORS (Cross-Origin Resource Sharing)}
    
const app = express(); //app contains the express instance
app.use(cors());       
app.use(express.json()); 

// Connect to MongoDB
mongoose.connect('mongodb://127.0.0.1:27017/localstorageDB', { //connect to mongodb by URL of mongodb
  useNewUrlParser: true,
  useUnifiedTopology: true
})
  .then(() => console.log('✅ Connected to MongoDB'))   //if connected successfully, log the message
  .catch(err => console.log('❌ MongoDB Error:', err)); //if not connected, log the error

// Define schema(structure) for the data to be saved in MongoDB
const dataSchema = new mongoose.Schema({
  name: String,
  email: String,
  username: String,
  email_: String
});

const DataModel = mongoose.model('Data', dataSchema);

// Route to save data
app.post('/save-data', async (req, res) => {  //endpoint to save data
  //req is the request object and res is the response object
  try {
    const newData = new DataModel(req.body);
    await newData.save();
    res.status(201).json({ message: 'Data saved to MongoDB ✅' });
  } catch (err) {
    res.status(500).json({ message: 'Failed to save data ❌' });
  }
});

// Start the server
app.listen(5000, () => { //listening on port 5000
  //callback function to execute when the server starts
  console.log('🚀 Server running at http://localhost:5000'); 
});

//connect
//schema
//model
//all steps