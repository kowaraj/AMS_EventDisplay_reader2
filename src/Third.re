let str = ReasonReact.string;

type sss = {
    i: int, 
    len: int
};

[@react.component]
let make = () => {
    let (sslist, setSSList) = React.useReducer(
        (_oldState, actionIsNewState) => actionIsNewState,  //state after action
        None // initial state
    );
    let (sss, setSSS) = React.useState( () => {i: 0, len: 0})
//    let (lastss, setLastSS) = React.useState( () => 0 )
    let (debug, setDebug) = React.useReducer(
        (_s, a) => a,
        true
    );


    let doFetchJSON = () => {
        //Js.log("fetching list of screenshots in json");
        Js.Promise.(
            Fetch.fetch("https://test-apashnin-ams.web.cern.ch/test-apashnin-ams/buffer_copied/ss_list.json") 
            |> then_(Fetch.Response.json)
            |> then_(json => {
                json 
                |> Decode.ss_list
                |> (fs => {Some(fs)}  |> resolve)
                })
        );
    };

    let callDoFetchJSON = () => {
        doFetchJSON()
        |> Js.Promise.then_( result => { 
            Js.log("doFetchJSON results are: "); 
            Js.log(result); 
            switch result { 
                | Some(r  : Decode.ss) => setSSS(s => {...s, len: List.length(r.fns)})
                | None => setSSS(s => {...s, len: 0})
            }
            //Js.log("ss_list len = " ++ string_of_int(sslistlen));
            setSSList(result);
            Js.Promise.resolve(); 
        } )
        |> ignore
    };

    React.useEffect0( () => {
        Js.log("Fetching data! ")
        callDoFetchJSON();        
        None // no destroying
    });
//    ,  [|lastss|]);

    // side effect: Preloading images from the fetched filenames
    React.useEffect1( () => {
        switch sslist {
        | Some(fetched_ss_list : Decode.ss) => 
        {
            // preloading...
            let ss_path = "https://test-apashnin-ams.web.cern.ch/test-apashnin-ams/buffer_copied/";
            let preload_ss = [%raw {|
                function(a) {
                    //console.log("preloading: " + a);
                    const img = new Image();
                    img.src = a;
                    return;
                }
                |}];

            List.map( 
                ss_i => {
                    preload_ss(ss_path ++ ss_i) |> ignore;
                    <p key=ss_i> {str(ss_i)} </p> 
                }, 
                fetched_ss_list.fns
            )
            |> Array.of_list
            |> React.array
            |> ignore;
            
            None;
        }
        | None => None;
        }
    }, 
    [|sslist|]);

    let fetchSS = (_e) => {
        Js.log("fetch SS");
        callDoFetchJSON();
    };
    let nextSS = (_e) => {
        Js.log("next SS");
        //setSSIndex( (ssindex + 1) mod sslistlen); 
        ();
    };
    let prevSS = (_e) => {
        Js.log("prev SS");
        //setSSIndex( (ssindex > 0) ? (ssindex - 1) : ssindex) ;
        ()
    };
    let timerCallbackOnTick = () => {
        setSSS( s => {...s, i: (s.len > 1) ? ((s.i + 1) mod s.len) : s.i})
    };

    let switchDebug = (_e) => {
        setDebug( !debug );
    };

    <div>
        <div hidden=(!debug)> 
            <>
            <InfiniteTimer cb=timerCallbackOnTick/>
            <button onClick={fetchSS}> {str("FETCH")} </button>
            <button onClick={nextSS}> {str("NEXT")} </button>
            <button onClick={prevSS}> {str("PREV")} </button> <br/>
            <p> {str("Total number of screenshots: " ++ string_of_int(sss.len) )}</p>
            <p> {str("Currently displayed is: " ++ string_of_int(sss.i) )}</p>
            </>
        </div>
        (
        switch sslist {
            | Some(fetched_ss_list : Decode.ss) => 
            {
                // display
                let ss_first = List.nth(fetched_ss_list.fns, sss.i);
                let ss_path = "https://test-apashnin-ams.web.cern.ch/test-apashnin-ams/buffer_copied/";

                <div> 
                    <div>
                        <br/>
                        {
                            Js.log("nth ("++string_of_int(sss.i) ++ ") element is: " ++ ss_path ++ ss_first);
                            <img src=(ss_path ++ ss_first) width="100%"/>          
                        }
                        <br/>
                        {
                            <div hidden=(!debug) height="300px">
                                <p> {str("List of fetched filenames:")} </p>
                                <div>
                                {
                                    List.map( 
                                        ss_i => {
                                            <p key=ss_i> {str(ss_i)} </p>
                                        }, 
                                        fetched_ss_list.fns
                                    )
                                    |> Array.of_list
                                    |> React.array;
                                }
                                </div>
                            </div>
                        }
                    </div>
                </div>;
            }
            | None => 
            {
                //Js.log(sslist);
                <div> 
                    {str("Failed to fetch the data")}
                </div>;
            }
        }
        )
        <>
            <button onClick={switchDebug}> {str("DEBUG")} </button>
        </>
    </div>
}
