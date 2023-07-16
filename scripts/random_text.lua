local json = require("json")

local random_texts = {
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
  "Sed ut perspiciatis unde omnis iste natus error sit voluptatem.",
  "In hac habitasse platea dictumst. Sed consequat tortor eget.",
  "Nullam condimentum, dolor id feugiat ullamcorper, neque mauris.",
  "Fusce ac felis a mauris condimentum luctus.",
  "Donec rutrum congue leo eget malesuada.",
  "Vestibulum ac diam sit amet quam vehicula elementum.",
  "Curabitur non nulla sit amet nisl tempus convallis quis ac lectus.",
  "Praesent sapien massa, convallis a pellentesque nec, egestas non nisi.",
  "Cras ultricies ligula sed magna dictum porta."
}

function init()
  -- Add any initialization code here
end

function get_data()
  local random_index = math.random(1, #random_texts)
  local random_text = random_texts[random_index]
  
  local data = { text = random_text }
  
  local json_data = json.encode(data)
  
  return json_data
end

function destroy()
  -- Add any cleanup code here
end
